#pragma once
#include <stdint.h>
#include "queue.h"

#define STD_DEVICE "/dev/input/js0"

#define PAD_LEFT_XNUM 3
#define PAD_LEFT_YNUM 4
#define PAD_RIGHT_XNUM 0
#define PAD_RIGHT_YNUM 1

typedef struct js_pad {
  uint8_t number;
  int16_t x;
  int16_t y;
} js_pad;

typedef struct js_button {
  uint8_t number;
  uint8_t state;
} js_button;

typedef struct js_handler {
  int fd;
  js_pad right_pad;
  js_pad left_pad;
} js_handler;

/** initialize the js_handler. Returns 0 if succesful, -1 otherwhise **/
int js_handler_initialize(js_handler*);

/** reads data from the joystick. each new event will be 
 *  enqueued in a given list.
 *  Operation is set in Blocking mode so the function will
 *  stand still until a new event is sent. **/
int js_handler_read(js_handler*);

/** destroy the passed js_handler **/
int js_handler_destroy(js_handler*);

