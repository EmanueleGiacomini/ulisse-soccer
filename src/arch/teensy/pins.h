#include <stdint.h>

typedef struct {
  volatile uint32_t* in_register;
  volatile uint32_t* out_register;
  volatile uint32_t* dir_register;
  uint8_t bit;

  // timer registers for PWM
  volatile uint32_t* tcc_register;
  volatile uint32_t* oc_register;
  const uint32_t com_mask;
} Pin;

// gotta change this
#define PINS_NUM 29 //TODO: Must check if this is right

extern const Pin pins[];