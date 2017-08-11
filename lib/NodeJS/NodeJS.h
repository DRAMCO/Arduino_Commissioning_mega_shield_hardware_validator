// NodeJS.h
#ifndef NodeJS_h
#define NodeJS_h

#include <Arduino.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>
#include "TimeLib.h"
#include "Timers.h"
#include <avr/wdt.h>
#include "Hex2Decimal.h"

#define DEBUG

// Pins used by ethernet shield
#define SCK       13
#define MISO      12
#define MOSI      11
#define SS        10

// Constants
#define MEASUREMENTS    250

#define MACLENGTH       12
#define PRIVLENGTH      1
#define RSSILENGTH      3
#define BRCSTLENGTH     24
#define BRCSTSENDLENGTH 16

// States for statemachine
typedef enum{
  NOTHING,
  HTTP_POLL,
  NODEJS_CONNECT,
  NODEJS_SEND_SCAN,
  BT_CONFIGURE_SCAN,
  BT_START_SCAN,
  BT_SCANNING,
  BT_CONFIGURE_ADVERTISE,
  BT_ADVERTISING,
  COMMISSIONED,
  RESET
} States;

class NodeJS{
  public:
    //functions
    NodeJS(Timers t, Hex2Decimal H2D, byte device_id);
    void NodeJSConnect(String BTMAC);
    void getBody(String * responsePointer);
    int getStatus(String * responsePointer);
    String HTTPPoll();
    String HTTPSendAdvertisement();
    States parseHTTPReply(String http);

    States getState();
    void setState(States s);
    States getPreviousState();
    void setPreviousState(States s);
    String getHTTPReply();
    void setHTTPReply(String H);

    //variables
    doubleLong MACToSend[MEASUREMENTS];
    int privToSend[MEASUREMENTS];
    int RSSIDecToSend[MEASUREMENTS];
    doubleLong brcstToSend[MEASUREMENTS];

  private:
    States state = NODEJS_CONNECT;
    States previousState = NOTHING;
    String HTTPReply;
    Timers timer;
    byte device_id;
    Hex2Decimal H2D;
    char txt[16];
};

#endif
