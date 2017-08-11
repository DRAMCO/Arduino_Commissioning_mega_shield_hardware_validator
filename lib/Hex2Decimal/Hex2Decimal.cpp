#include <Arduino.h>
#include "Hex2Decimal.h"

/*
** Constructor
*/
Hex2Decimal::Hex2Decimal(){}

/*
** Pasre an ASCII-HEX string (max 4 characters) and return a DEC integer
** Parameters: String RSSI
*/
int Hex2Decimal::hexToInt(String RSSI){
  signed int value = 0;
  if(RSSI[0] == '-'){
    for(uint8_t j = 1 ; j < RSSI.length(); j++){
      if(RSSI[j] >= 0x30 && RSSI[j] <= 0x39){       //between 0x30 (0) && 0x39 (9) substract 0x30 for conversion to int
        value = value * 0x10 + RSSI[j] - 0x30;
      }else if(RSSI[j] >= 0x41 && RSSI[j] <= 0x46){ //between 0x41 (A) && 0x46 (F) substract 0x37 for conversion to int
        value = value * 0x10 + RSSI[j] - 0x37;
      }
    }
    value *= -1;
    if(value > -20) value = 0;
  } else {
    for(uint8_t j = 0 ; j < RSSI.length(); j++){
      if(RSSI[j] >= 0x30 && RSSI[j] <= 0x39){       //between 0x30 (0) && 0x39 (9) substract 0x30 for conversion to int
        value = value * 0x10 + RSSI[j] - 0x30;
      }else if(RSSI[j] >= 0x41 && RSSI[j] <= 0x46){ //between 0x41 (A) && 0x46 (F) substract 0x37 for conversion to int
        value = value * 0x10 + RSSI[j] - 0x37;
      }
    }
  }
  return value;
}

/*
** Pasre an ASCII-HEX string (max 8 characters) and return a DEC unsigned long
** Parameters: String number
*/
unsigned long Hex2Decimal::hexToLong(String number){
  unsigned long value = 0;
  for(uint8_t j = 0 ; j < number.length(); j++){
    if(number[j] >= 0x30 && number[j] <= 0x39){       //between 0x30 (0) && 0x39 (9) substract 0x30 for conversion to int
      value = value * 0x10 + number[j] - 0x30;
    }else if(number[j] >= 0x41 && number[j] <= 0x46){ //between 0x41 (A) && 0x46 (F) substract 0x37 for conversion to int
      value = value * 0x10 + number[j] - 0x37;
    }
  }
  return value;
}

/*
** Pasre an ASCII-HEX string (max 16 characters) and return a DEC unsigned long
** Parameters: String number
*/
doubleLong Hex2Decimal::hexToDoubleLong(String number){
  doubleLong value = {.firstLong = 0, .secondLong = 0};
  String firstString;
  String secondString;
  int length = number.length();
  int length1 = 0;
  int length2 = 0;
  if(length > 15){
    length2 = 9;
    length1 = 8;
  }else if(length <= 15 && length > 7){
    length2 = 8;
    length1 = length - length2;
  }else if(length <= 7){
    length2 = length;
    length1 = 0;
  }
  value.firstLong = hexToLong(number.substring(length-length1-length2, length-length2));
  value.secondLong = hexToLong(number.substring(length-length2, length));
  return value;
}
