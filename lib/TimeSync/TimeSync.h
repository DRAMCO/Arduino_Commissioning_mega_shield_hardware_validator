// TimeSync.h
#ifndef TimeSync_h
#define TimeSync_h

#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include "TimeLib.h"


void printDigits(int digits);
void digitalClockDisplay();


/*-------- NTP code ----------*/
time_t getNtpTime(EthernetUDP Udp);

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address, EthernetUDP Udp);

#endif
