// Hex2Decimal.h
#ifndef Hex2Decimal_h
#define Hex2Decimal_h

#include <Arduino.h>


typedef struct doubleLong_{
  unsigned long firstLong;
  unsigned long secondLong;
} doubleLong;

class Hex2Decimal{
  public:
    Hex2Decimal();
    int hexToInt(String RSSI);
    unsigned long hexToLong(String RSSI);
    doubleLong hexToDoubleLong(String RSSI);
  private:
};

#endif
