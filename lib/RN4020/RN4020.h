#ifndef RN4020_h
#define RN4020_h

#include <Arduino.h>

#define DEBUG

// RN4020 pins
#define RX3             15
#define TX3             14
#define WAKESW          53
#define MLDP            52
#define CONN            51
#define MLDPEV          50
#define ACTIVITY        49
#define WAKEHW          48

// data string lengths
#define MACLENGTH       12
#define PRIVLENGTH      1
#define RSSILENGTH      3
#define BRCSTLENGTH     24
#define BRCSTSENDLENGTH 16

class RN4020{
  public:
    // Functions
    RN4020();
    void init();
    void read();
    void readNoDelay();
    String readString();
    void reset();
    void setupAdvertise();
    void advertise();
    void setupScan();
    void getMACInit();
    void setupScanDevices();
    void stopScan();
    int device_id = 0;

    // Setters and getters
    String getMAC();
    void setMAC(String M);
    String getPriv();
    void setPriv(String p);
    String getRSSIHex();
    void setRSSIHex(String RSSI);
    String getBrcst();
    void setBrcst(String b);

  private:
    String MAC;
    String priv;
    String RSSIHex;
    String brcst;
};

#endif
