/**
 * @brief Library to interface with 74HC595 Shift Register
 * 
 * @file shiftreg.h
 * @author Emanuele Giacomini
 * @date 2018-04-19
 */

#pragma once

#include <Arduino.h>
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

#define MOT1_DIRA 0
#define MOT1_DIRB 1
#define MOT2_DIRA 2
#define MOT2_DIRB 3
#define MOT3_DIRA 4
#define MOT3_DIRB 5 
#define MOT4_DIRA 6
#define MOT4_DIRB 7 

typedef struct shiftreg_data {
    uint8_t led_data;
    uint8_t motor_data;
} shiftreg_data;

class ShiftRegister {
public:
    ShiftRegister();
    void setData(DATA_TYPE type, uint8_t *data);
    void getData(DATA_TYPE type, uint8_t *data);
    void updateData(DATA_TYPE type, uint8_t *data);
    inline void update();
    
protected:
    int _latch_pin, _data_pin, _clock_pin;
    shiftreg_data _data;
};

uint8_t setBit(uint8_t data, DATA_INDEX index, bool s);
bool getBit(uint8_t data, DATA_INDEX index);


