#pragma once
#include "common/packet_handler.h"
#include "common/ulisse_packets.h"

#ifdef __cplusplus
extern "C" {
#endif
  struct UlisseClient;
  
  // creates a new ulisse client, opening a serial connection on device at the selected baudrate
  struct UlisseClient* UlisseClient_init(const char* device, uint32_t baudrate);

  // destroyes a previously created ulisse client
  void UlisseClient_destroy(struct UlisseClient* cl);

  // sends a packet
  // if timeout ==0, the packet sending is deferred
  // otherwise it enables a synchronous operation that waits for timeout packets to be received
  PacketStatus UlisseClient_sendPacket(struct UlisseClient* cl, PacketHeader* p, int timeout);
  
  //gets a packet from Ulisse client
  PacketStatus UlisseClient_get(struct UlisseClient* cl, PacketHeader* dest, PacketType type);

  /** High level functionalities, multithreaded */
  
  // TBA

  PacketStatus UlisseClient_sync(UlisseClient* cl, int cycles) {
    for(int c=0; c<cycles; c++) {
      pthread_mutex_lock(&cl->write_mutex);
      _flushBuffer(cl);
      // To Be Continued...
    }
  }
#ifdef __cplusplus
}
#endif