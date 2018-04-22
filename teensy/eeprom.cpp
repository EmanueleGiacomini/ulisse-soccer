/**
 * @brief 
 * 
 * @file eeprom.cpp
 * @author your name
 * @date 2018-04-22
 */
#include "eeprom.h"
#include <Arduino.h>
#include <EEPROM.h>
#include "global.h"

template <typename T>
void getEEPROM(int address, T& value) {
  char* c=reinterpret_cast<char*>(&value);
  int a=address;
  for (int i=0; i<sizeof(T); i++){
    *c=EEPROM.read(a);
    c++;
    a++;
  }
}

template <typename T>
void putEEPROM(int address, const T& value) {
  const char* c=reinterpret_cast<const char*>(&value);
  int a=address;
  for (int i=0; i<sizeof(T); i++){
    EEPROM.write(a, *c);
    c++;
    a++;
  }
}

template <typename T>
void updateEEPROM(int address, const T& value) {
  const char* c=reinterpret_cast<const char*>(&value);
  int a=address;
  for (int i=0; i<sizeof(T); i++){
    char new_char=EEPROM.read(a);
    if (new_char!=*c)
      EEPROM.write(a, *c);
    c++;
    a++;
  }
}

void initEEPROM() {

}

void loadParamsFromEEPROM(uint8_t param_type) {

}

void writeParamsToEEPROM(uint8_t param_type) {

}