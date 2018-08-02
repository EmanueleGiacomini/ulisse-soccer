#include <stdlib.h>
#include <Arduino.h>
#include "queue.h"
#include "foo.h"
#include <avr/io>

int main() {
  int* bar = foo();
  FTM1_SC |= (1<<TOIE);
  return 0;
}