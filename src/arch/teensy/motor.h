/**
 * @brief 
 * 
 * @file motor.h
 * @date 2018-04-22
 */
#pragma once
#include <stdint.h>
#include "shiftreg.h"

class Motor {
public:
    // Initializes a detached motor
    Motor();
    // Initializes a motor and let arduino setup the pins given
    Motor(int pwm_pin, uint8_t shiftreg_offset);
    void init(int pwm_pin, uint8_t shiftreg_offset);

    inline int pwmPin() const {return _pwm_pin;}
    inline int16_t maxPWM() const {return _max_pwm;}
    inline void setMaxPWM(int16_t max_pwm) {_max_pwm = max_pwm;}

    inline void setPWMSpeed(int16_t pwm_speed) {
        _pwm_speed=pwm_speed;
        _pwm_speed=(_pwm_speed<_max_pwm?_pwm_speed:_max_pwm);
        _pwm_speed=(_pwm_speed>-_max_pwm?_pwm_speed:-_max_pwm);
    }

    /**
     * @brief Flush all values to the hardware
     */
    void update();

protected:
    int _pwm_pin;
    uint8_t _shiftreg_offset;
    int16_t _pwm_speed;
    int16_t _max_pwm;

};