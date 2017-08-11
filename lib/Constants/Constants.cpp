#include <Arduino.h>
#include "Constants.h"

/*
** Constructor
*/
Constants::Constants(){
  _initPins();
}

/*
** Pinmodes for all the pins
*/
void Constants::_initPins(){

  // Leds & switches
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);

  pinMode(SW1 , INPUT_PULLUP);
  pinMode(SW2 , INPUT_PULLUP);
  pinMode(DSW1, INPUT_PULLUP);
  pinMode(DSW2, INPUT_PULLUP);
  pinMode(DSW3, INPUT_PULLUP);
  pinMode(DSW4, INPUT_PULLUP);
  pinMode(DSW5, INPUT_PULLUP);
  pinMode(DSW6, INPUT_PULLUP);
}

byte Constants::readDipswitch(){
  byte value = 0;
  value |= digitalRead(DSW1) << 0;
  value |= digitalRead(DSW2) << 1;
  value |= digitalRead(DSW3) << 2;
  value |= digitalRead(DSW4) << 3;
  value |= digitalRead(DSW5) << 4;
  value |= digitalRead(DSW6) << 5;
  return value;
}
