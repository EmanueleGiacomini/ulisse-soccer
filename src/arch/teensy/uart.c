#include <kinetis.h>
#include "uart.h"

#define UART_BUFFER_SIZE 128

void setBaud115200(void) {
#define BAUD 115200
// Setup uart0 registers for baudrate of 115200
#undef BAUD
}

typedef struct UART {
  int tx_buffer[UART_BUFFER_SIZE];
  volatile uint8_t tx_start;
  volatile uint8_t tx_end;
  volatile uint8_t tx_size;

  int rx_buffer[UART_BUFFER_SIZE];
  volatile uint8_t rx_start;
  volatile uint8_t rx_end;
  volatile uint8_t rx_size;
  
  int baud;
  int uart_num; // hardware uart;
} UART;

static UART uart_0;


struct UART* UART_init(const char* device __attribute__((unused)), uint32_t baud) {
  UART* uart=&uart_0;
  uart->uart_num=0;

  switch(baud) {
    case 115200: setBaud
  }

  // Enable clock
  SIM_SCGC4 |= SIM_SCGC4_UART0;
  


}