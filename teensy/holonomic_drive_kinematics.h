/**
 * @brief 
 * 
 * @file holonomic_drive_kinematics.h
 * @date 2018-04-22
 */
#pragma once
#include <stdint.h>

class HolonomicDriveKinematics {
public:
    HolonomicDriveKinematics(float *theta_motors);
    void setParams(float *theta_motors);
    inline float getMotorComp(uint8_t motor);
    inline void setDirection(int16_t dir) {
        
    }
protected:
    float *_theta_motors;
    float _x, _y;
    float *_motors_velocity;
};