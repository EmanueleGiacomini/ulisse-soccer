#include "../include/encoder.h"
#include <stdint.h>

#define NUM_ENCODERS 4
#define ENCODER_MASK 0x0F // Needs to be changed

/**
 * @brief Here how's the encoders are connected
 * to the teensy.
 * Enc_1 => PTD0(ENCA),PTD1(ENCB) 0b00000AB
 * Enc_2 => PTE0(ENCA),PTE1(ENCB)
 * Enc_3 => PTA12(ENCA),PTA5(ENCB)
 * Enc_4 => PTC9(ENCA),PTC0(ENCB) * 
 */

typedef struct {
  uint16_t current_value;
  uint16_t sampled_value;
  uint8_t pin_state;
} Encoder;

Encoder _encoders[NUM_ENCODERS] = {
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0}
};

// initializes the encoder subsystem
void Encoder_init(void) {
  cli();  // Deactivates interrupt routine
  
}

// samples the encoder, saving the respective values
// in a temporary storage
void Encoder_sample(void);

// returns the number of the encoder
uint8_t Encoder_numEncoders(void);

// returns the value of an encoder, when sampled with
// the Encoder_sample();

uint16_t Encoder_getValue(uint8_t num_encoder);