/**
 * @brief 
 * 
 * @file eeprom.h
 * @author your name
 * @date 2018-04-22
 */
#pragma once
#include "stdint.h"

const uint32_t firmware_version=0xF1CACCB;

const int eeprom_version_address=0x0000;
const int eeprom_system_param_address=0x0010;

//! initializes the eeprom with firmware version
//! if it is the first time this function is called, it writes once the stuff in EEPROM  
void initEEPROM();

//! reads the default params from eeprom
void loadParamsFromEEPROM(uint8_t param_type);

//! writes the default params to eeprom
void writeParamsToEEPROM(uint8_t param_type);
