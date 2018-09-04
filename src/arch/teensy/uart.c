#include <kinetis.h>
#include "uart.h"

#define UART_BUFFER_SIZE 128
#define SYSCLK 30

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

void usb_init() {
  //* Setup USB device *//
  
  // assume 48 MHz clock already running
  SIM_SCGC4 |= SIM_SCGC4_USBOTG;

  // Clear all ISR flags
  USB0_ISTAT = 0xFF;
  USB0_ERRSTAT = 0xFF;
  USB0_OTGISTAT = 0xFF;

  // enable USB
  USB0_CTL = USB_CTL_USBENSOFEN;
  USB0_USBCTRL = 0;

  // enable reset interrupt
  USB0_INTEN = USB_INTEN_USBRSTEN;
  USB0_CONTROL = USB_CONTROL_DPPULLUPNONOTG;
  
  /**
  SIM_SCGC4 |= SIM_SCGC4_USBOTG;
  PORTB_PCR0 |= 0 | PORT_PCR_MUX(1);  // PTB0 as I/O pin
  // SIM Configuration
  SIM_SCGC4|=(SIM_SCGC4_USBOTG); // USB Clock Gating
  SIM_SCGC6|=(SIM_SCGC4_USBOTG); // USB Clock Gating

  // USB pre-initialization 
  USB0_USBTRC0 |= USB_USBTRC_USBRESET;
  while(USB0_USBTRC0 & USB_USBTRC_USBRESET);  // Wait for reset to happen
  USB0_ISTAT |= USB_ISTAT_USBRST;
  // Enable USB Reset Interrupt
  USB0_INTEN |= USB_INTEN_USBRSTEN; //USBRST Interrupt Enable
  USB0_USBCTRL = 0x00;
  //USB0_USBTRC0 |= 0x40; bit not documented :(
  USB0_CTL |= USB_CTL_USBENSOFEN;
  **/
}

struct UART* UART_init(const char* device __attribute__((unused)), uint32_t baud) {
  uint16_t ubd, brfa;
  uint16_t temp;

  // Configure UART struct
  UART* uart=&uart_0;
  uart->uart_num=0;

  // Enable UART pins by configuring PORTx_PCRn register
  // UART0 ->  PORTA_PCR0 (Uart0_rx) mux(2)
  // UART0 ->  PORTA_PCR1 (Uart0_tx) mux(2)

  usb_init();

  // rx: PullUp Enable to keep it on 3.3V 
  PORTA_PCR0 |= PORT_PCR_MUX(2) | PORT_PCR_PE;
  // tx: SlewRate Enable, Drive Strenght Enable
  PORTA_PCR1 |= PORT_PCR_MUX(2) | PORT_PCR_SRE | PORT_PCR_DSE;

  // Enable the clock on UART0
  SIM_SCGC4 |= SIM_SCGC4_UART0;
  // Disable Receiver and Transmitter while Uart is begin initialized
  UART0_C2 &= ~(UART_C2_RE | UART_C2_TE);

  // Configure the UART for 8-bit mode, no parity
  // We need all default settings, so entire register is cleared
  UART0_C1 = 0;

  // Calculate baud settings
  ubd = (uint16_t)((SYSCLK*1000)/(baud * 16));

  // Save off the current value of the UARTx_BDH except for the SBR (first 5 bits)
  temp = UART0_BDH & ~(0x1F);
  UART0_BDH = temp | ((ubd & 0x1F00) >> 8);
  UART0_BDL = (uint8_t) (ubd & 0xFF);

  // Determine if a fractional divider is needed to get closer to the baud rate
  brfa = (((SYSCLK*32000)/(baud * 16)) - (ubd * 32));

  // Save off the current value of UART0_C4 except for BRFA (BaudRateFineAdjust)
  temp = UART0_C4 & ~(0x1F);
  UART0_C4 = temp | UART_C4_BRFA(brfa);

  // Enable transmitter and receiver
  UART0_C2 |= UART_C2_RE | UART_C2_TE;
  return &uart_0;
}

uint8_t UART_getChar(struct UART* uart __attribute__((unused))) {
  while(!(UART0_S1 & UART_S1_RDRF));
  return UART0_D;
}
