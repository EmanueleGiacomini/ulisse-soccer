#include "packet_handler.h"
#include <stdio.h>

uint8_t PacketHandler_initialize(PacketHandler* h) {
  h->rx_buffer=0;
  h->rx_buffer_end=0;
  h->rx_bytes_to_read=0;
  h->tx_size=0;
  h->tx_start=0;
  h->tx_end=0;
  return 0;
}