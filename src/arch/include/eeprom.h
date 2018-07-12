#pragma once
#include <stdint.h>

// initializes the eeprom subsystem
void EEPROM_init(void);

// reads in dest size bytes starting from eeprom src
void EEPROM_read(void* dest, const uint16_t src, uint16_t size);

// writes starting from eeprom dest cell 'size' bytes from src
void EEPROM_write(uint16_t dest, const void* src, uint16_t size);