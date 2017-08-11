#include <Arduino.h>
#include <Wire.h>
#include "Constants.h"
#include "RN4020.h"
#include "TimeLib.h"
#include "TimeSync.h"
#include "Timers.h"
#include "Hex2Decimal.h"
#include "NodeJS.h"
#include "DS3232RTC.h"
#include "Adafruit_MPL3115A2.h"
#include "M24SR.h"
#include "FreeRAM.h"

#define DEBUG

// Constructors
Constants pins;
RN4020 bluetooth;
Timers timer;
Hex2Decimal H2D;
NodeJS nodejs(timer, H2D, pins.readDipswitch());
Adafruit_MPL3115A2 baro;
M24SR m24sr;

EthernetUDP Udp;
unsigned int localPort = 8888;                                // local port to listen for UDP packets

String URL = "http://10.128.69.169:3000/id";

// Setup
void setup(){
  bluetooth.init();
  bluetooth.reset();
  bluetooth.getMACInit();
  bluetooth.setupScan();
  Serial.println(pins.readDipswitch(), DEC);
  Serial.println(F("Setup done, let's get started"));

  // Barometer test
  Serial.println(F("Barometer test"));
  if (! baro.begin()) {
      Serial.println("Couldnt find sensor");
      return;
    }

  float pascals = baro.getPressure();
  // Our weather page presents pressure in Inches (Hg)
  // Use http://www.onlineconversion.com/pressure.htm for other units
  Serial.print(pascals/3377); Serial.println(F(" Inches (Hg)"));

  float altm = baro.getAltitude();
  Serial.print(altm); Serial.println(F(" meters"));

  float tempC = baro.getTemperature();
  Serial.print(tempC); Serial.println(F("*C"));

  delay(250);

  // M24SR test
  m24sr._setup();
  displayFreeRAM();
  NdefMessage message = NdefMessage();
  message.addUriRecord(&URL[0]);
  displayFreeRAM();
  m24sr.writeNdefMessage(&message);
  delay(250);
}

// Loop
void loop(){
  switch(nodejs.getState()){
    case NOTHING:
      if(timer.getISRTriggered()){
        nodejs.setPreviousState(nodejs.getState());
        nodejs.setState(HTTP_POLL);
      }
      break;
    case HTTP_POLL:
      #ifdef DEBUG
        Serial.println(F("HTTP_POLL!"));
      #endif
      nodejs.setHTTPReply(nodejs.HTTPPoll());
      //test stuff

      Serial.print(baro.getAltitude()); Serial.println(" meters");
      digitalClockDisplay();
      //
      nodejs.setPreviousState(nodejs.getState());
      nodejs.setState(nodejs.parseHTTPReply(nodejs.getHTTPReply()));
      break;
    case NODEJS_CONNECT:
      nodejs.NodeJSConnect(bluetooth.getMAC());
      Udp.begin(localPort);
      Serial.println("waiting for sync");
      RTC.set(getNtpTime(Udp));
      setSyncProvider(RTC.get);
      digitalClockDisplay();
      timer.setupTimer();
      nodejs.setPreviousState(nodejs.getState());
      nodejs.setState(NOTHING);
      break;
    default:;
      #ifdef DEBUG
        Serial.println(F("statemachine default, something went wrong"));
      #endif
      nodejs.setState(NOTHING);
      bluetooth.readNoDelay();
  }
}
