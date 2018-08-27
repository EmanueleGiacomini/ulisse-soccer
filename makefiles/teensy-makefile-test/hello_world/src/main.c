#include <stdlib.h>
#include <Arduino.h>
#include "queue.h"
#include "foo.h"

void setup() {
  int i;
  for(i=2;i<32;++i)
    pinMode(i, 1);
    digitalWrite(i, 1);
}

void loop() {

}
// pin 13: PTC5

int main() {
  setup();
  while(1)
    loop();
}
