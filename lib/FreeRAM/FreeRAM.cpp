#include <Arduino.h>
#include "FreeRAM.h"

int freeRam() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void displayFreeRAM() {
  Serial.print(F("\r\nfree RAM: "));
  Serial.println(freeRam(), DEC);
}
