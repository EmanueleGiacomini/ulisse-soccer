#pragma once
#include "packet_header.h"
#include "packet_operations.h"
#include <stdint.h>

#define RX_BUFFER_SIZE 64
#define TX_BUFFER_SIZE 64
/**
 * @brief PacketHandler is responsible of receiving and sending
 * packets.
 * It is possible to parse a packet by using the 
 * _rxByte function, and it is possible to send one by using
 * the sendPacket function.
 */


#ifdef __cplusplus 
extern "C" {
#endif

struct PacketHandler;
// Definition of PacketHandlerRxFn as pointer to a function
// that takes PacketHandler* and a uint8_t as parameters and
// returns a PacketStatus
typedef PacketStatus (*PacketHandlerRxFn)(struct PacketHandler*, uint8_t c);

typedef struct PacketHandler {
  PacketOperations *operations[PACKET_TYPE_MAX];

  // Stuff used while receiving stuff
  uint8_t* rx_buffer;
  uint8_t* rx_buffer_end;
  PacketOperations* rx_current_op;
  PacketHeader* rx_current_packet;
  PacketSize rx_bytes_to_read;
  PacketHandlerRxFn rxFn;

  uint8_t tx_buffer[PACKET_SIZE_MAX];
  int tx_start;
  int tx_end;
  int tx_size;
}PacketHandler;

void PacketHandler_initialize(PacketHandler* h);

#ifdef __cplusplus 
}
#endif

void PacketHandler_initialize(PacketHandler* h);

PacketStatus PacketHandler_installPacket(PacketHandler* h, PacketOperations* ops); 
PacketStatus PacketHandler_uninstallPacket(PacketHandler* h, PacketType type);

// _rxByte will interpret an incoming byte c
PacketStatus PacketHandler_rxByte(PacketHandler* handler, uint8_t c);
// _txByte takes out from the tx_buffer the first byte.
uint8_t PacketHandler_txByte(PacketHandler* h);
// sendPacket fill tx_buffer with the outgoing packet
// CARE: require _flushBuffer from (orazio_client) to really
// send the packet on the other side 
PacketStatus PacketHandler_sendPacket(PacketHandler* h, PacketHeader* header);