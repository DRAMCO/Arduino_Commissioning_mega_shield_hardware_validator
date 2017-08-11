#include <Arduino.h>
#include "Timers.h"
#include <avr/wdt.h>

volatile bool ISRTriggered = false;

/*
** Constructor
*/
Timers::Timers(){}

ISR(TIMER1_COMPA_vect){
  ISRTriggered = true;
}

void Timers::softwareReset(uint8_t prescaler) {
  // start watchdog with the provided prescaler
  wdt_enable(prescaler);
  // wait for the prescaler time to expire
  // without sending the reset signal by using
  // the wdt_reset() method
  while(1){}
}

void Timers::setupTimer(){          //datasheet atmega2560
  cli();                            //disable global interupts
  TCCR1A = 0;                       // set entire TCCR1A register to 0
  TCCR1B = 0;                       // same for TCCR1B
  TCNT1  = 0;                       //initialize counter value to 0
  OCR1A = 62499;                    //set compare match register to 62499 (+ 1 for reset) => counts till 62,5k => (8 / 16 MHz * 62500 k = 1 second and one tick takes 16 µs
  TCCR1B |= 1 << CS12;              //prescaler op 256 (datasheet page 157, CSn2, = 1);
  TCCR1B |= 1 << WGM12;             //set compare timer mode (datasheet page 133 table);
  TIMSK1 |= 1 << OCIE1A;            //enable time compare interrupt
  sei();                            //enable global interrupts
}

void Timers::disableTimerInterrupt(){
  #ifdef DEBUG
    Serial.println("Disable timer");
  #endif
  TCCR1B &= ~(1 << CS12);           //prescaler op 256 (datasheet page 157, CSn2, = 1);
  TCCR1B &= ~(1 << WGM12);
  TIMSK1 &= ~(1 << OCIE1A);
}

void Timers::enableTimerInterrupt(){
  #ifdef DEBUG
    Serial.println("enable timer");
  #endif
  TCCR1B |= 1 << CS12;               //prescaler op 256 (datasheet page 157, CSn2, = 1);
  TCCR1B |= 1 << WGM12;              //set compare timer mode (datasheet page 133 table);
  TIMSK1 |= 1 << OCIE1A;             //enable time compare interrupt
}

bool Timers::getISRTriggered(){
  return ISRTriggered;
}

void Timers::setISRTriggered(bool b){
  ISRTriggered = b;
}
