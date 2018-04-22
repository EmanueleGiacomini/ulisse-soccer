/**
 * @brief Contains platform specific implementations
 * and initializes the platform
 * 
 * @file global.h
 * @author your name
 * @date 2018-04-22
 */

#pragma once
#include "motor.h"
#include "shiftreg.h"

/*********** MOTOR SETUP AND GLOBALS ***********/
const uint8_t num_motors=4;
extern ShiftRegister shiftregister;
extern Motor motors[num_motors];
