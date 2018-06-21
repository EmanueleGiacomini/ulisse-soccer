#include "packet_handler.h"
#include "buffer_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void PacketHandler_initialize(PacketHandler* h) {
  h->rx_buffer=0;
  h->rx_buffer_end=0;
  h->rx_current_op=0;
  h->rx_current_packet=0;
  h->rx_bytes_to_read=0;
  h->rxFn=_rxAA;

  h->tx_start=0;
  h->tx_end=0;
  h->tx_size=0;
}

PacketStatus PacketHandler_installPacket(PacketHandler* h, PacketOperations* ops) {
  if(ops->type >= PACKET_TYPE_MAX) {
    return PacketTypeOutOfBounds;
  }
  if(h->operations[ops->type] != 0) {
    return PacketInstallError;
  }
  h->operations[ops->type] = ops;
  return Success;
}

PacketStatus PacketHandler_uninstallPacket(PacketHandler* h, PacketType type) {
  if(type >= PACKET_TYPE_MAX) {
    return PacketTypeOutOfBounds;
  }
  if(h->operations[type] == 0) {
    return PacketUninstallError;
  }
  h->operations[type]=0;
  return Success;
}

static inline void _txBuf_set(PacketHandler* h, uint8_t c) {
  if(h->tx_size >= PACKET_SIZE_MAX) return;
  h->tx_buffer[h->tx_end]=c;
  BUFFER_PUT(h->tx, PACKET_SIZE_MAX);
}

static inline uint8_t _txBuf_get(PacketHandler* h) {
  if(!h->tx_size) return 0;
  uint8_t c = h->tx_buffer[h->tx_start];
  BUFFER_GET(h->tx, PACKET_SIZE_MAX)
}

uint8_t PacketHandler_txByte(PacketHandler* h) {
  return _txBuf_get(h);
}

PacketStatus PacketHandler_sendPacket(PacketHandler* h, PacketHeader* header) {
  int tx_free = PACKET_SIZE_MAX-h->tx_size;
  int real_size = header->size+3;
  if(tx_free<real_size){
    return TxBufferFull;
  }
  // Checksum stuff
  // I will use it for learning purposes:
  // Each transmission will begin with 0xAA followed by 0x55
  // In case this is not true while receiving the packet, we know
  // something went wrong.
  _txBuf_set(h, 0xAA);
  _txBuf_set(h, 0x55);
  //
  uint8_t size = header->size;
  uint8_t* buf = (uint8_t*)header;
  while(size) {
    _txBuf_set(h, *buf);
    size--;
    buf++;
  }
  // Later on 0xFF must be changed with computed checksum
  _txBuf_set(h, 0xFF);
  return Success;
}

// What happens here is that the first time read() is called
// we will recive the first byte of data, and the func _rxAA
// will test if the first byte equals 0xAA.
// If this is true then the rxFn(ReceiveFunction) will be updated
// to parse the new byte.

PacketStatus _rxAA(PacketHandler* h, uint8_t c) {
  if(c==0xAA) {
    h->rxFn=_rx55;
    return SyncAA;
  }
  return Unsync;
}

PacketStatus _rx55(PacketHandler* h, uint8_t c) {
  if(c==0x55) {
    h->rxFn=_rxType;
    return Sync55;
  }
  h->rxFn=_rxAA;
  return Unsync;
}

// After the first two bytes, we find the header->type
PacketStatus _rxType(PacketHandler* h, uint8_t c) {
  if(c>=PACKET_TYPE_MAX) {
    h->rxFn=_rxAA;
    return Unsync;
  }
  h->rx_current_op=h->operations[c];
  // if the packet is of an unknown type
  if(!h->rx_current_op) {
    h->rxFn=_rxAA;
    return UnknownType;
  }
  // If everything went fine, lets move on.
  h->rxFn=_rxSize;
  return SyncType;
}

PacketStatus _rxSize(PacketHandler* h, uint8_t c) {
  // PACKET_SIZE_ANY sounds like a standard packet size
  // If receiving packet's size differs from PACKET_SIZE_ANY or
  // the expected packet dimension. then an error occurred
  if(h->rx_current_op->size!=PACKET_SIZE_ANY && h->rx_current_op->size != c) {
    h->rxFn=_rxAA;
    printf("invalid size, type: %d, desired: %d, received: %d\n",
      h->rx_current_op->type,
      h->rx_current_op->size,
      c);
    return InvalidSize;
  }
  // Initialize rx_current_packet
  h->rx_current_packet = (*h->rx_current_op->initialize_buffer_fn)
    (h->rx_current_op->type,
    h->rx_current_op->size,
    h->rx_current_op->initialize_buffer_args);
  if(!h->rx_current_packet) {
    h->rxFn=_rxAA;
    return RxBufferError;
  }
  // Calculate bytes to read
  // the payload(c) - header.size + seq.size (missing header part)
  // (why subtract header.size ??)
  h->rx_bytes_to_read=c-sizeof(PacketHeader)+sizeof(PacketSeq);
  // rx_buffer will point to rx_current_packet
  h->rx_buffer=(uint8_t*)h->rx_current_packet;
  // Reconstruct the header with the expected
  // values
  h->rx_buffer[0]=h->rx_current_op->type;
  h->rx_buffer[1]=h->rx_current_op->size;
  h->rx_buffer_end=h->rx_buffer+2;
  // to the next part !
  h->rxFn=_rxPayload;
  return SyncSize;  
}
// This function will be repeated for each byte of
// the payload.
PacketStatus _rxPayload(PacketHandler* h, uint8_t c) {
  *h->rx_buffer_end=c;
  ++h->rx_buffer_end;
  --h->rx_bytes_to_read;
  if(!h->rx_bytes_to_read) {
    // We read the damn packet !!!!
    // lets go on the last part!
    h->rxFn=_rxChecksum;
    return SyncPayloadComplete;
  }
  return SyncPayload;
}

PacketStatus _rxChecksum(PacketHandler* h, uint8_t c) {
  // Even if its called checksum, this has nothing
  // to do with it... yet.

  //reset the receive handler stuff
  h->rxFn=_rxAA;
  h->rx_buffer=0;
  h->rx_buffer_end=0;
  // this last character is the checksum value
  // We wont calculate it though
  if(c==0xFF) {
    if(h->rx_current_op->on_recive_fn) {
      // If rx_current_op has a receive func
      // it is time to execute it!
      (*h->rx_current_op->on_recive_fn)
        (h->rx_current_packet,
        h->rx_current_op->on_recive_args);
    }    
    return SyncChecksum;
  }
  return ChecksumError;
}