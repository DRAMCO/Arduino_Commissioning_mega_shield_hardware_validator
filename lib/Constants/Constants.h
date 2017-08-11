// Constants.h
#ifndef Constants_h
#define Constants_h

#include <Arduino.h>

// Analog sensors
#define LUX             15
#define CO              9
#define NO2             8
#define AUDIO           6
#define O3              0

// Communication pins
#define TX              1
#define RX              0
#define TX1             18
#define RX1             19
#define TX2             16
#define RX2             17
#define SDA             20
#define SCL             21

// GPS
#define GPS_INT         35

// Leds & switches
#define LED1            37
#define LED2            39
#define LED3            41
#define LED4            43
#define LPWM            45

#define SW1             32
#define SW2             34
#define DSW6            36
#define DSW5            38
#define DSW4            40
#define DSW3            42
#define DSW2            44
#define DSW1            46

class Constants{
  public:
    Constants();
    byte readDipswitch();
  private:
    void _initPins();
};

#endif
