// Timers.h
#ifndef Timers_h
#define Timers_h

#include <Arduino.h>

class Timers{
  public:
    Timers();
    void softwareReset(uint8_t prescaler);
    void setupTimer();
    void disableTimerInterrupt();
    void enableTimerInterrupt();
    bool getISRTriggered();
    void setISRTriggered(bool b);
  private:
};

#endif
