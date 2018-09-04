#include <kinetis.h>
#include "digio.h"
#include "eeprom.h"
#include "uart.h"

#define TO1 9000000
#define TO2 5000000
#define TO3 1

#define LED_OFFSET 32


void ledToggle(int state) {
  DigIO_setValue(13, state);
}

void systime_init() {
  // 16pF and 8pF load capacitors active
  OSC0_CR = OSC_SC16P | OSC_SC8P;
  // Enabling the oscillator for 8 MHz crystal
  MCG_C2 = MCG_C2_RANGE0(1) | MCG_C2_HGO0 | MCG_C2_EREFS;

  // Select ext oscillator
  MCG_C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(3);

  // wait for oscillator to initialize
  ledToggle(1);
  while(!(MCG_S & MCG_S_OSCINIT0)) {}
  ledToggle(0);

  // wait for Reference clock to switch to external reference
  ledToggle(1);
  while(MCG_S & MCG_S_IREFST) {}
  ledToggle(0);

  // wait for MCGOUT to switch over to the external reference clock
  ledToggle(1);
  while(((MCG_S & MCG_S_CLKST_MASK) >> 2) != 0x2) {}
  ledToggle(0);

  // now configure the PLL and move to PBE mode
  MCG_C5 = MCG_C5_PRDIV0(1);

  // Set the VDIV field to 0 which is x24, giving 4x24 = 96 MHz
  MCG_C6 = MCG_C6_CME0 | MCG_C6_PLLS;

  // wait until the source of the PLLS clock has switched to the PLL
  ledToggle(1);
  while(!(MCG_S & MCG_S_PLLST)) {}
  ledToggle(0);

  // wait until the PLL has achieved lock
  while(!(MCG_S & MCG_S_LOCK0)) {}

  // set up the SIM clock dividers BEFORE switching to the PLL to ensure
  // the system clock speeds are in specs
  SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV2(1)
              | SIM_CLKDIV1_OUTDIV3(1) | SIM_CLKDIV1_OUTDIV4(3);

  // Transition into PEE by setting CLKS to 0
  MCG_C1 &= ~0xC0;
  
  // Wait for MCGOUT to switch over to the PLL
  ledToggle(1);
  while (((MCG_S & MCG_S_CLKST_MASK) >> 2) != 0x3){}
  ledToggle(0);

  // The USB clock divider in the System Clock Divider Reg 2
  // Should be configured to generate the 48 MHz clock

  SIM_CLKDIV2 |= SIM_CLKDIV2_USBDIV(1);
  
}

void _usb_init() {
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
}

int main(void) {
  PORTC_PCR5 |= PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1);
  DigIO_setDirection(13, Output);
  ledToggle(1);
  systime_init();
  
  _usb_init();
  UART_init("", 115200);
  ledToggle(0);
  


  //UART_init("", 115200);
  //ledToggle(1);
  //uint8_t recv=UART_getChar(NULL);
  //ledToggle(recv);

  /*
  /// Led blink
  uint32_t ctr1 = 0, ctr2 = 0, ctr3 = 0;
  int8_t state = 0;

  PORTC_PCR5 |= PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1);
  DigIO_setDirection(13, Output);
  while(1) {
    ctr1++;
    if(ctr1 > TO1) {
      ctr1=0;
      ctr2++;
      if(ctr2 > TO2) {
        ctr2=0;
        ctr3++;
        if(ctr3 > TO3) {
          ctr3=0;
          state = !state;
          ledToggle(state);
        }
        
      }
    }    
  }
  */
  return 0;
}