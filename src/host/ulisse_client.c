#include "ulisse_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include "serial_linux.h"


typedef struct UlisseClient {
  PacketHandler packet_handler;
  // not sure what it does
  uint16_t global_seq;

  // these are the system variables, updated by the serial communication
  ResponsePacket response;
  EndEpochPacket end_epoch;
  SystemStatusPacket system_status;
  JointParamPacket joint_param;
  JointStatusPacket joint_status;

  // file descriptor of the serial port
  int fd;
  uint8_t packet_buffer[PACKET_SIZE_MAX];

  //sanity check on trasmission
  PacketSize packet_sizes[PACKET_TYPE_MAX];
  pthread_mutex_t write_mutex;
  pthread_mutex_t read_mutex;

  JointControlPacket joint_control_packet;
  
}UlisseClient;
// returns the pointer to a preallocated buffer where packet_handler can
// save the incoming packets.
static PacketHeader* _initializeBuffer(PacketType type, PacketSize size, void* arg) {
  UlisseClient* client=(UlisseClient*)arg;
  return (PacketHeader*)client->packet_buffer;
}

// this handler is called whenever a packet is complete
// no deferred action will take place
static PacketStatus _copyToBuffer(PacketHeader* p, void* args) {
  memcpy(args, p, p->size);
  return Success;
}

// Install a new packet operation inside UlisseClient by invoking
// PacketHandler_installPacket
static PacketStatus _installPacketOp(UlisseClient* cl,
          PacketHeader* dest,
          PacketType type,
          PacketSize size) {
  PacketOperations* ops=(PacketOperations*)malloc(sizeof(PacketOperations));
  ops->type=type;
  ops->size=size;
  ops->initialize_buffer_fn=_initializeBuffer;
  ops->initialize_buffer_args=cl;
  ops->on_recive_fn=_copyToBuffer;
  ops->on_recive_args=dest;
  PacketStatus install_result = PacketHandler_installPacket(&cl->packet_handler, ops);
  if(install_result!=Success) {
    printf("Error in installing ops\n");
    free(ops);
    exit(0);      
  }
  return install_result;
}

// flush packet_handler.tx_buffer trough serial port
static void _flushBuffer(UlisseClient* cl) {
  // Until there are bytes to send, keep sending.
  while(cl->packet_handler.tx_size) {
    uint8_t c = PacketHandler_txByte(&cl->packet_handler);
    write(cl->fd, &c, 1);
  }
}

// read packet from serial port
static void _readPacket(UlisseClient* cl) {
  volatile int packet_complete=0;
  // Keep reading new bytes until the whole packet is parsed.
  while(!packet_complete) {
    uint8_t c;
    int n=read(cl->fd, &c, 1);
    if(n) {
      // why ??
      fflush(stdout);
      PacketStatus status = PacketHandler_rxByte(&cl->packet_handler, c);
      if(0 && status<0) {
        printf("Error: %d\n", status);
      }
      packet_complete=(status==SyncChecksum);
    }
  }
}

// creates a new ulisse client, opening a serial connection on device at the selected baudrate
UlisseClient* UlisseClient_init(const char* device, uint32_t baudrate) {
  // Setup the serial interface.
  int fd=serial_open(device);
  if(fd<0)
    return 0;
  if(serial_set_interface_attribs(fd, baudrate, 0) < 0)
    return 0;
  serial_set_blocking(fd, 1);
  if(!fd)
    return 0;  
  
  // build the UlisseClient
  UlisseClient* cl = (UlisseClient*)malloc(sizeof(UlisseClient));
  cl->global_seq=0;
  cl->fd=fd;

  // Initializes all the packets to send.
  

  // initialize the packet system
  /**
   * ResponsePacket response;
   * SystemStatusPacket system_status;
   * JointParamPacket joint_param;
   * JointStatusPacket joint_status;
   */
  _installPacketOp(cl, (PacketHeader*)&cl->response, RESPONSE_PACKET_ID, sizeof(cl->response));
  _installPacketOp(cl, (PacketHeader*)&cl->system_status, SYSTEM_STATUS_PACKET_ID, sizeof(cl->system_status));
  _installPacketOp(cl, (PacketHeader*)&cl->joint_param, JOINT_PARAM_PACKET_ID, sizeof(cl->joint_param));
  _installPacketOp(cl, (PacketHeader*)&cl->joint_status, JOINT_STATUS_PACKET_ID, sizeof(cl->joint_status));

  // initialize the response
  cl->response.header.type=RESPONSE_PACKET_ID;
  cl->response.header.size=sizeof(cl->response);
  cl->response.header.seq=0;
  cl->response.p_seq=0;
  cl->response.p_type=PACKET_TYPE_MAX;

  // initializes the outbound type/packet size
  memset(&cl->packet_sizes, 0, sizeof(cl->packet_sizes));
  cl->packet_sizes[PARAM_CONTROL_PACKET_ID]=sizeof(ParamControlPacket);
  cl->packet_sizes[JOINT_PARAM_PACKET_ID]=sizeof(JointParamPacket);
  cl->packet_sizes[JOINT_CONTROL_PACKET_ID]=sizeof(JointControlPacket);
  
  // initializes the r/w mutex
  pthread_mutex_init(&cl->write_mutex, NULL);
  pthread_mutex_init(&cl->read_mutex, NULL);
  return cl;
}

// destroyes a previously created ulisse client
void UlisseClient_destroy(struct UlisseClient* cl) {
  close(cl->fd);
  for(int i=0;i<PACKET_TYPE_MAX; i++) {
    if(cl->packet_handler.operations[i]) {
      free(cl->packet_handler.operations[i]);
    }
  }
  pthread_mutex_destroy(&cl->write_mutex);
  pthread_mutex_destroy(&cl->read_mutex);
  free(cl);
}

static PacketStatus _sendPacket(struct UlisseClient* cl, PacketHeader* p) {
  // Increase the global_seq counter
  ++cl->global_seq;
  // Checks on packet type/size
  PacketType type=p->type;
  if(type>=PACKET_TYPE_MAX)
    return UnknownType;
  PacketSize expected_size=cl->packet_sizes[type];
  if(!expected_size)
    return UnknownType;
  if(p->size!=expected_size)
    return InvalidSize;
  p->seq=cl->global_seq;
  // Let the PacketHandler handle the job
  return PacketHandler_sendPacket(&cl->packet_handler, p);
}

// sends a packet
// if timeout ==0, the packet sending is deferred
// otherwise it enables a synchronous operation that waits for timeout packets to be received
PacketStatus UlisseClient_sendPacket(struct UlisseClient* cl, PacketHeader* p, int timeout) {
  PacketStatus send_result=GenericError;
  // non-blocking operation
  if(!timeout) {
    pthread_mutex_lock(&cl->write_mutex);
    send_result=_sendPacket(cl, p);
    pthread_mutex_unlock(&cl->write_mutex);
    return send_result;
  }

  // blocking operation
  // no one else can write or read, we read packets
  // until timeout or until the response is received
  pthread_mutex_lock(&cl->write_mutex);
  pthread_mutex_lock(&cl->read_mutex);
  send_result=_sendPacket(cl, p);
  if(send_result!=Success) {
    goto safe_exit;
  }
  _flushBuffer(cl);
  uint16_t awaited_seq=p->seq;
  uint16_t awaited_type=p->type;
  send_result = Timeout;
  while(timeout>0) {
    _readPacket(cl);
    if(cl->response.p_type==awaited_type
    && cl->response.p_seq==awaited_seq) {
      send_result = Success;
      break;
    }
    timeout--;
  }
 safe_exit:
  pthread_mutex_unlock(&cl->read_mutex);
  pthread_mutex_unlock(&cl->write_mutex);
  return send_result;
}

//gets a packet from Ulisse client
PacketStatus UlisseClient_get(struct UlisseClient* cl, PacketHeader* dest, PacketType type) {
  if(type>=PACKET_TYPE_MAX)
    return PacketTypeOutOfBounds;
  // all packets received have been registered in the handler ops
  // and the var on_recive_args of the buffer points to the correct
  // variable
  const PacketOperations* ops=cl->packet_handler.operations[type];
  if(!ops)
    return UnknownType;
  // What are we trying to achieve here ? 
  pthread_mutex_lock(&cl->read_mutex);
  memcpy(dest, ops->on_recive_args, ops->size);
  pthread_mutex_unlock(&cl->read_mutex);
  return Success;
}

PacketStatus UlisseClient_sync(UlisseClient* cl, int cycles) {
  for(int c=0; c<cycles; c++) {
    pthread_mutex_lock(&cl->write_mutex);
    _flushBuffer(cl);
    pthread_mutex_unlock(&cl->write_mutex);
    // While a new end_epoch packet is sent, keep reading
    // data
    uint16_t current_seq=cl->end_epoch.seq;
    do {
      pthread_mutex_lock(&cl->read_mutex);
      _readPacket(cl);
      pthread_mutex_unlock(&cl->read_mutex);
    } while(current_seq==cl->end_epoch.seq);
    printf("Sync! current_seq: %d\n", cl->end_epoch.seq);
  }
}

PacketStatus UlisseClient_readConfiguration(UlisseClient* cl, int timeout) {
  printf("Querying params\n");
  // building a new ParamControlPacket 
  ParamControlPacket query= {
    {
      .type=PARAM_CONTROL_PACKET_ID,
      .size=sizeof(ParamControlPacket),
      .seq=0
    },
    .action=ParamRequest,
    .param_type=ParamSystem // Query for ParamSystem
  };
  PacketStatus status = UlisseClient_sendPacket(cl, (PacketHeader*)&query, timeout);
  printf(" [System] Status: %d\n", status);
  if(status!=Success)
    return status;
  
  // Query for ParamJoints
  query.param_type=ParamJoints;
  status = UlisseClient_sendPacket(cl, (PacketHeader*)&query, timeout);
  printf(" [Joints] Status: %d\n", status);
  if(status!=Success)
    return status;
  
  // Query for ParamDrive
  query.param_type=ParamDrive;
  status = UlisseClient_sendPacket(cl, (PacketHeader*)&query, timeout);
  printf(" [Drive] Status: %d\n", status);
  if(status!=Success)
    return status;
  
  printf("Done\n");
  return status;
}

PacketStatus UlisseClient_setJointControl(UlisseClient* cl,
              uint8_t joint_num,
              uint16_t speed) {
  cl->joint_control_packet.joints[joint_num].speed=speed;
}