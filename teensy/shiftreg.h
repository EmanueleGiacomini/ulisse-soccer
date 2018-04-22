/**
 * @brief Library to interface with 74HC595 Shift Register
 * 
 * @file shiftreg.h
 * @author Emanuele Giacomini
 * @date 2018-04-19
 */

#pragma once

#include <stdint.h>

typedef uint8_t DATA_TYPE;
typedef uint8_t DATA_INDEX;


#define LED_DATA 0
#define MOTOR_DATA 1

#define LED_RED 0
#define LED_WHITE 1 
#define LED_BLUE 2
#define LED_YELLOW 3 
#define LED_FRONT 4
#define LED_LEFT 5
#define LED_BACK 6
#define LED_RIGHT 7

#define MOT0_DIRA 0
#define MOT0_DIRB 1
#define MOT1_DIRA 2
#define MOT1_DIRB 3
#define MOT2_DIRA 4
#define MOT2_DIRB 5 
#define MOT3_DIRA 6
#define MOT3_DIRB 7 

typedef struct shiftreg_data {
    uint8_t led_data;
    uint8_t motor_data;
} shiftreg_data;

class ShiftRegister {
public:
    ShiftRegister();
    /**
     * @brief Initialize the shift register's pins
     * 
     * @param latch_pin 
     * @param data_pin 
     * @param clock_pin 
     */
    void init(uint8_t latch_pin, uint8_t data_pin, uint8_t clock_pin);
    /**
     * @brief Set one of the shift register's pins to output s
     * 
     * @param type 
     * @param offset 
     * @param s 
     */
    void setData(DATA_TYPE type, DATA_INDEX offset, bool s);
    /**
     * @brief Gets the data byte from the shift register.
     * the type shall be LED_DATA in order to get the led byte and
     * MOTOR_DATA to get the motor byte
     * 
     * @param type 
     * @param offset 
     * @return uint8_t 
     */
    uint8_t getData(DATA_TYPE type, DATA_INDEX offset);
    /**
     * @brief If needed, sends and update message to the shift register
     * 
     */
    void update();
    
protected:
    int _latch_pin, _data_pin, _clock_pin;
    bool _fupdate;
    shiftreg_data _data;
};

bool getBit(uint8_t *data, DATA_INDEX index);
bool setBit(uint8_t *data, DATA_INDEX index, bool s);


