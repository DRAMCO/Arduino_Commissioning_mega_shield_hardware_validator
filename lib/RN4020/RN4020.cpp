#include <Arduino.h>
#include <RN4020.h>

/*
** Constructor
*/
RN4020::RN4020(){
  // Setup BT pins
  pinMode(WAKESW  , OUTPUT);
  pinMode(MLDP    , OUTPUT);
  pinMode(CONN    , INPUT_PULLUP);
  pinMode(MLDPEV  , INPUT_PULLUP);
  pinMode(ACTIVITY, INPUT_PULLUP);
  pinMode(WAKEHW  , OUTPUT);

  // String reserves
  MAC.reserve(MACLENGTH);
  priv.reserve(PRIVLENGTH);
  RSSIHex.reserve(RSSILENGTH);
  brcst.reserve(BRCSTLENGTH);
}

void RN4020::init(){
  // enable serial
  Serial.begin(115200);
  Serial3.begin(115200);
}

void RN4020::read(){
  char temp;
  delay(100);
  while(Serial3.available() && temp != '\n'){
    temp = Serial3.read();
    #ifdef DEBUG
      Serial.write(temp);
    #endif
  }
}

void RN4020::readNoDelay(){
  char temp;
  while(Serial3.available() && temp != '\n'){
    temp = Serial3.read();
    #ifdef DEBUG
      Serial.write(temp);
    #endif
  }
}

String RN4020::readString(){
  String readData = "";
  while(Serial3.available() > 0){
    char temp = Serial3.read();
    readData.concat(temp);
  }
  return readData;
}

void RN4020::reset(){
  digitalWrite(WAKESW, HIGH);
  digitalWrite(WAKEHW, LOW);
  digitalWrite(MLDP, LOW);
  read();
  Serial3.println(F("R,1"));             //reboot so actions take effect
  delay(1500);                           //delay after reboot important!
  read();                                //read "reboot" and "CMD"
  read();
  read();
  digitalWrite(WAKESW, LOW);
  digitalWrite(WAKEHW, HIGH);
  digitalWrite(MLDP, LOW);
  delay(500);
  readNoDelay();
  read();
}

void RN4020::setupAdvertise(){
  // Setup RN4020
  digitalWrite(WAKESW, HIGH);
  digitalWrite(WAKEHW, LOW);
  digitalWrite(MLDP, LOW);
  delay(500);
  read();                                //read the "CMD" + empty buffer
  readNoDelay();
  readNoDelay();
  //Serial.println("+");                 //echo
  read();
  Serial3.println("SF,1");               //factory reset
  read();
  Serial3.println("SS,C0000000");        //set supported services
  read();
  Serial3.println("SR,10000000");        //set features (peripheral, mldp enabled)
  read();
  read();
  Serial3.println("R,1");                //reboot so actions take effect
  delay(1500);                           //delay after reboot important!
  read();                                //read "reboot" and "CMD"
  digitalWrite(WAKESW, LOW);
  digitalWrite(WAKEHW, HIGH);
  digitalWrite(MLDP, LOW);
  delay(500);
  readNoDelay();
  read();
}

void RN4020::advertise(){
  digitalWrite(WAKESW, HIGH);
  digitalWrite(WAKEHW, LOW);
  digitalWrite(MLDP, LOW);
  delay(500);
  Serial3.println("SP,4");
  read();
  Serial3.println("N,000000"+String(device_id)+"4");
  read();
  Serial3.println("A");
  read();
  read();                              //read "reboot" and "CMD"
  digitalWrite(WAKESW, LOW);
  digitalWrite(WAKEHW, HIGH);
  digitalWrite(MLDP, LOW);
  delay(500);
  readNoDelay();
  read();
}

void RN4020::setupScan(){
  digitalWrite(WAKESW, HIGH);
  digitalWrite(WAKEHW, LOW);
  digitalWrite(MLDP, LOW);
  delay(1000);
  readNoDelay();
  Serial3.println(F("derp"));            //first command always gives error (don't know why, get the ERR message back)
  read();
  readNoDelay();
  //Serial.println("+");                 //echo
  read();
  Serial3.println(F("V"));               //check device information
  read();
  Serial3.println(F("SF,1"));            //factory reset
  read();
  Serial3.println(F("SS,C0000000"));     //set supported services
  read();
  Serial3.println(F("SR,90000000"));     //set features (central, MLDP enabled)
  read();
  read();
  Serial3.println(F("R,1"));             //reboot so actions take effect
  delay(1500);                           //delay after reboot important!
  read();                                //read "reboot" and "CMD"
  read();                                //read "reboot" and "CMD"
  digitalWrite(WAKESW, LOW);
  digitalWrite(WAKEHW, HIGH);
  digitalWrite(MLDP, LOW);
  delay(500);
  readNoDelay();
  read();
}

void RN4020::getMACInit(){
  digitalWrite(WAKESW, HIGH);
  digitalWrite(WAKEHW, LOW);
  digitalWrite(MLDP, LOW);
  delay(1000);
  readNoDelay();
  Serial.println(F("D"));
  Serial3.println(F("D"));
  delay(100);
  String data = readString();
  if(data.startsWith("BTA=")){
    MAC = data.substring(4, 16);
  };
  digitalWrite(WAKESW, LOW);
  digitalWrite(WAKEHW, HIGH);
  digitalWrite(MLDP, LOW);
  delay(500);
  readNoDelay();
  read();
}

//TODO
/*states*/ void RN4020::setupScanDevices(){
  //states newState = BT_SCANNING;
  String error = "";
  #ifdef DEBUG
    Serial.println("Setup BT scan");
  #endif
  digitalWrite(WAKESW, HIGH);
  digitalWrite(WAKEHW, LOW);
  digitalWrite(MLDP, LOW);
  read();
  delay(200);
  Serial3.println(F("J,1"));           //observer mode
  delay(100);
  read();
  read();
  Serial3.println(F("F"));             //scan environment for device
  delay(100);
  error = readString();
  //Serial.print(F("error string: "));
  //Serial.println(error);
  //if(error.startsWith("ERR")){
    //Serial.println(F("done goofed"));
    //newState = BT_CONFIGURE_SCAN;
  //}
  digitalWrite(WAKESW, LOW);
  digitalWrite(WAKEHW, HIGH);
  digitalWrite(MLDP, LOW);
  delay(500);
  readNoDelay();
  read();
  //return newState;
}

void RN4020::stopScan(){
  #ifdef DEBUG
    Serial.println("Stop BT scan");
  #endif
  digitalWrite(WAKESW, HIGH);
  digitalWrite(WAKEHW, LOW);
  digitalWrite(MLDP, LOW);
  delay(100);
  Serial3.println(F("X"));           //observer mode
  read();                            //read "reboot" and "CMD"
  digitalWrite(WAKESW, LOW);
  digitalWrite(WAKEHW, HIGH);
  digitalWrite(MLDP, LOW);
  delay(500);
  read();
  readNoDelay();
  read();
}

// Setters and getters
String RN4020::getMAC(){
  return MAC;
}

void RN4020::setMAC(String M){
  MAC = M;
}

String RN4020::getPriv(){
    return priv;
}

void RN4020::setPriv(String p){
    priv = p;
}

String RN4020::getRSSIHex(){
  return RSSIHex;
}

void RN4020::setRSSIHex(String RSSI){
  RSSIHex = RSSI;
}

String RN4020::getBrcst(){
  return brcst;
}

void RN4020::setBrcst(String b){
  brcst = b;
}
