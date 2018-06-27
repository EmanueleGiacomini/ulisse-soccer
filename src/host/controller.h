#pragma once
#include <stdint.h>

#define STD_DEVICE "/dev/input/js0"

typedef struct js_pad {
  uint8_t number;
  double x;
  double y;
} js_pad;

/** initialize the jshandler and returns the file descriptor to read the joystick **/
int jshandler_begin();

