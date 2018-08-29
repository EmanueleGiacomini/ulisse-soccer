#include "kinetis.h"
#include <stdlib.h>
#include "queue.h"
#include "foo.h"

// pin 13: PTC5

#define OF_VALUE 1000000

void setLed(int state) {
  if(state) {
    GPIOC_PTOR |= 1 << 5;
  } else {
    GPIOC_PTOR &= ~(1<<5);
  }
}

int main() {
  // Set LED pin as an output
  PORTC_PCR5 = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1);
  GPIOC_PDDR |= 1 << 5;
  uint32_t _counter = 0;
  uint32_t counter = 0;
  uint8_t state = 0;

  uint8_t* ptr = (uint8_t*)calloc(10, sizeof(uint8_t));
  while(1) {
    if(++_counter > OF_VALUE) {
      ++counter;
      if(counter > 5000000) {
        counter = 0;
        state = !state;
        setLed(state);
      }      
    }    
  }
}
