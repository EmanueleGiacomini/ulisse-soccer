/**
 * @brief 
 * 
 * @file motor.cpp
 * @author your name
 * @date 2018-04-22
 */

#include <Arduino.h>
#include "global.h"
#include "motor.h"

Motor::Motor() {
    _pwm_pin = -1;
    _shiftreg_offset = UINT8_MAX;
    _max_pwm = 255;
}

Motor::Motor(int pwm_pin, uint8_t shiftreg_offset) {
    _pwm_pin = pwm_pin;
    _shiftreg_offset = shiftreg_offset;
    _max_pwm = 255;
}

void Motor::init(int pwm_pin, uint8_t shiftreg_offset) {
    _pwm_pin = pwm_pin;
    _shiftreg_offset = shiftreg_offset;

    pinMode(pwm_pin, OUTPUT);
}

void Motor::update() {
    if(_pwm_speed > 0) {
        analogWrite(_pwm_pin, _pwm_speed);
        shiftregister.setData(MOTOR_DATA, (2*_shiftreg_offset), 1);
        shiftregister.setData(MOTOR_DATA, (2*_shiftreg_offset)+1, 0);
    } else {
        analogWrite(_pwm_pin, -_pwm_speed);
        shiftregister.setData(MOTOR_DATA, (2*_shiftreg_offset), 0);
        shiftregister.setData(MOTOR_DATA, (2*_shiftreg_offset)+1, 1);
    }
    shiftregister.update();
}

