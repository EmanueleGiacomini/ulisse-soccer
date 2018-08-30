#include <kinetis.h>
#include "digio.h"
#include "eeprom.h"

#define TO1 9000000
#define TO2 5000000
#define TO3 1

#define LED_OFFSET 32


void ledToggle(int state) {
  DigIO_setValue(13, state);
}

int main(void) {
  uint32_t ctr1 = 0, ctr2 = 0, ctr3 = 0;
  uint8_t temp = 0;
  //uint8_t state = 0b11010101;

  // Set pin 13
  PORTC_PCR5 |= PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1);
  DigIO_setDirection(13, Output);

  // Initialize EEPROM
  EEPROM_init();
  
  
  uint8_t state = 0;
  EEPROM_write(LED_OFFSET, &state, sizeof(uint8_t));
  
  state = 0;
  EEPROM_read(&state, LED_OFFSET, sizeof(uint8_t));

  ledToggle(state);

  /*
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
          int _state = state >> temp;
          ledToggle(_state & 0x1);
          temp += 1;
          if(temp >= 8)
            break;
        }        
      }
    }    
  } 
  */   


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