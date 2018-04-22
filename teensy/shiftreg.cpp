/**
 * @brief 
 * 
 * @file shiftreg.cpp
 * @author your name
 * @date 2018-04-19
 */

#include "shiftreg.h"
#include <Arduino.h>


bool getBit(uint8_t data, DATA_INDEX index) {
    return ((data>>index) & 1);
}

bool setBit(uint8_t *data, DATA_INDEX index, bool s) {
    if(getBit(data, index) != s) {
        if(s) *data |= (1<<index);            
        else *data &= ~(1<<index);
        return 1;
    }    
    return 0;
}

ShiftRegister::ShiftRegister() {
    _data.led_data = 0x0;
    _data.motor_data = 0x0;
    _fupdate = 1;
}
void ShiftRegister::init(uint8_t latch_pin, uint8_t data_pin, uint8_t clock_pin) {
    pinMode(latch_pin, OUTPUT);
    pinMode(data_pin, OUTPUT);
    pinMode(clock_pin, OUTPUT);
    _latch_pin = latch_pin;
    _data_pin = data_pin;
    _clock_pin = clock_pin;
}

void ShiftRegister::setData(DATA_TYPE type, DATA_INDEX offset, bool s) {
    if(type == LED_DATA) _fupdate = setBit(&_data.led_data, offset, s);
    else if(type == MOTOR_DATA) _fupdate = setBit(&_data.motor_data, offset, s);
}
uint8_t ShiftRegister::getData(DATA_TYPE type, DATA_INDEX offset) {
    if(type == LED_DATA) return _data.led_data;
    else if(type == MOTOR_DATA) return _data.motor_data;
    return 0;
}

void ShiftRegister::update() {
    if(!_fupdate) return;
    digitalWrite(_latch_pin, HIGH);
    shiftOut(_data_pin, _clock_pin, MSBFIRST, _data.led_data);
    shiftOut(_data_pin, _clock_pin, MSBFIRST, _data.motor_data);
    digitalWrite(_latch_pin, LOW);
    _fupdate = 0;
}