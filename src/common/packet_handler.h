#pragma once
#include "packet_header.h"
#include "packet_operations.h"
#include <stdint.h>


#ifdef __cplusplus 
extern "C" {
#endif

typedef struct PacketHandler {    
  uint8_t *rx_buffer;
  uint16_t bytes_to_read;


  uint8_t *tx_buffer;
  uint16_t bytes_to_write;

}PacketHandler;


// initializes an empty packet handler
uint8_t PacketHandler_initialize(PacketHandler* h);

// installs the manager for packet operations
//uint8_t PacketHandler_installPacket(PacketHandler* h, PacketOperations* ops);

// removes a packet
uint8_t PacketHandler_uninstallPacket(PacketHandler* h, PacketType type);

// sends a packet. If returning failure, the packet is not sent
uint8_t PacketHandler_sendPacket(PacketHandler* handler, PacketHeader* header);

/* functions to be connected to the UART*/

// sends a byte if available in the tx buffer
uint8_t PacketHandler_txByte(PacketHandler* h);

/* NOT NEEDED BECAUSE IM TOO LAZY TO USE CHECKSUM */
// processes a byte if available from the rx buffer
//PacketStatus PacketHandler_rxByte(PacketHandler* handler, uint8_t c);

#ifdef __cplusplus 
}
#endif

