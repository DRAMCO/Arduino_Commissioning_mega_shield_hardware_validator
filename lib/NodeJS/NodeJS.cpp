#include <Arduino.h>
#include "NodeJS.h"

EthernetClient client;
IPAddress server(10,128,69,169);                              //10,128,69,169 of 192,168,0,102

byte mac[20][6] = {{0x90, 0xA2, 0xDA, 0x0E, 0xF4, 0xBA},
                   {0x90, 0xA2, 0xDA, 0x0E, 0xF4, 0xBB},
                   {0x90, 0xA2, 0xDA, 0x0E, 0xF4, 0xBC},
                   {0x90, 0xA2, 0xDA, 0x0E, 0xF4, 0xBD},
                   {0x90, 0xA2, 0xDA, 0x0E, 0xF4, 0xBE},
                   {0x90, 0xA2, 0xDA, 0x0F, 0x20, 0xBF},
                   {0x90, 0xA2, 0xDA, 0x0E, 0xF4, 0xC1},
                   {0x90, 0xA2, 0xDA, 0x0E, 0xF4, 0xC2},
                   {0x90, 0xA2, 0xDA, 0x0E, 0xF4, 0xC3},
                   {0x90, 0xA2, 0xDA, 0x0E, 0xF4, 0xC4},
                   {0x90, 0xA2, 0xDA, 0x0E, 0xF4, 0xC5},
                   {0x90, 0xA2, 0xDA, 0x0F, 0x20, 0xC6},
                   {0x90, 0xA2, 0xDA, 0x0E, 0xF4, 0xC7},
                   {0x90, 0xA2, 0xDA, 0x0E, 0xF4, 0xC8},
                   {0x90, 0xA2, 0xDA, 0x0E, 0xF4, 0xC9},
                   {0x90, 0xA2, 0xDA, 0x0E, 0xF4, 0xCA},
                   {0x90, 0xA2, 0xDA, 0x0F, 0x20, 0xCB},
                   {0x90, 0xA2, 0xDA, 0x0E, 0xF4, 0xCC},
                   {0x90, 0xA2, 0xDA, 0x0E, 0xF4, 0xCD},
                   {0x90, 0xA2, 0xDA, 0x0F, 0x20, 0xCE}};

/*
** Constructor
*/
NodeJS::NodeJS(Timers t, Hex2Decimal H, byte id){
  timer = t;
  device_id = id;
  H2D = H;
};

void NodeJS::NodeJSConnect(String BTMAC){
Serial.println(F("Connect to server"));
  String httpResponse;
  // setup ethernet
  if (Ethernet.begin(mac[device_id]) == 0) {
    #ifdef DEBUG
      Serial.println(F("Failed to configure Ethernet using DHCP"));
    #endif
    // no point in carrying on, so do nothing forevermore:
    for(;;);
  }
  // print your local IP address:
  Serial.println(Ethernet.localIP());

  // setup connection to server
  if (client.connect(server, 3000)) {
    #ifdef DEBUG
      Serial.println(F("connected"));
    #endif
    delay(100);
    //client.write('a');
    // Make a HTTP request:
    String url = "GET /devices/";
    url.concat(device_id);
    url.concat(F("/"));
    url.concat(BTMAC);
    client.println(url);
    client.println(F("Host: 10.128.69.169"));
    client.println(F("Connection: close"));
    client.println();
    httpResponse = "";
    //get response (delay 5 ms to give time for reply)
    delay(50);
    while (client.available()) {
      char c = client.read();
      httpResponse.concat(c);
    }
    getBody(&httpResponse);
    #ifdef DEBUG
      Serial.println(F("Http request sent and closed"));
      Serial.println(F("Http response: "));
      Serial.println(httpResponse);
      Serial.println(F("End of http response."));
      /*getBody(&httpResponse);
      Serial.println("Body of response: ");
      Serial.println(httpResponse);
      Serial.println("End of body");*/
    #endif
  } else {
    #ifdef DEBUG
      Serial.println(F("connection failed"));
      timer.softwareReset(WDTO_60MS);
    #endif
  }
  client.stop();
}

void NodeJS::getBody(String * responsePointer) {
  // Get body out of response, the empty line in the HTTP packet is where
  // the body begins
  #ifdef DEBUG
    Serial.println(getStatus(responsePointer));
  #endif
  if (getStatus(responsePointer) == 200) {
    int newLinePosition = responsePointer->indexOf(F("\r\n\r\n"));
    *responsePointer = responsePointer->substring(newLinePosition + 4);
    responsePointer->trim(); // Trim whitespace at the end and the beginning from body
  } else {
    *responsePointer = String("");
  }
}

int NodeJS::getStatus(String * responsePointer) {
  // Get status of HTTP message
  byte spacePosition = responsePointer->indexOf(' ');
  String statusString = responsePointer->substring(spacePosition + 1, spacePosition + 4);
  return statusString.toInt();
}

String NodeJS::HTTPPoll(){                          //do a proper poll
  String httpResponse;
  if (client.connect(server, 3000)) {
    #ifdef DEBUG
      Serial.println(F("connected"));
    #endif
    delay(100);
    //client.write('a');
    // Make a HTTP request:
    String url = "GET /poll/";
    url.concat(device_id);
    #ifdef DEBUG
      Serial.println(url);
    #endif
    client.println(url);
    client.println(F("Host: 10.128.69.169"));
    client.println(F("Connection: close"));
    client.println();
    httpResponse = "";
    //get response (delay 5 ms to give time for reply)
    delay(50);
    while (client.available()) {
      char c = client.read();
      httpResponse.concat(c);
    }
    getBody(&httpResponse);
    #ifdef DEBUG
      Serial.println("Body of response: ");
      Serial.println(httpResponse);
      Serial.println("End of body");
    #endif
  } else {
    #ifdef DEBUG
      Serial.println("connection failed");
    #endif
  }
  client.stop();
  timer.setISRTriggered(false);
  return httpResponse;
}

String NodeJS::HTTPSendAdvertisement(){
  String httpResponse = "";
  String content = "";
  int length = (MACLENGTH + RSSILENGTH + BRCSTSENDLENGTH + 28) * MEASUREMENTS + 1; //add 28 for json overhead
  if (client.connect(server, 3000)) {
    #ifdef DEBUG
      Serial.println(F("connected"));
    #endif
    delay(100);
    //client.write('a');
    // Make a HTTP request:
    String url = "/measurement/";
    url.concat(device_id);
    #ifdef DEBUG
      Serial.println(url);
    #endif
    client.print(F("POST "));
    client.print(url);
    client.println(F(" HTTP/1.1"));
    client.println(F("Host: 10.128.69.169"));
    client.println(F("Content-Type:application/json; charset=UTF-8"));
    client.println(F("Connection: close"));
    client.print(F("Content-Length: "));
    client.println(length);
    client.println();
    client.print('[');
    for(int i = 0; i < MEASUREMENTS; i++){
      client.print('{');
      client.print("\"MAC\":\"");
      sprintf(txt, "%04lX", MACToSend[i].firstLong);
      client.print(txt);
      sprintf(txt, "%08lX", MACToSend[i].secondLong);
      client.print(txt);
      client.print("\",");
      client.print("\"RSSI\":");
      sprintf(txt, "%2d", RSSIDecToSend[i]);
      client.print(txt);
      client.print(",");
      client.print("\"MSG\":\"");
      sprintf(txt, "%08lX", brcstToSend[i].firstLong);
      client.print(txt);
      sprintf(txt, "%08lX", brcstToSend[i].secondLong);
      client.print(txt);
      client.print("\"}");
      if(i < MEASUREMENTS-1){
        client.print(',');
      }
    }
    client.print(']');
    client.println("");
    httpResponse = "";
    //get response (delay 5 ms to give time for reply)
    delay(1000);
    while (client.available()) {
      char c = client.read();
      httpResponse.concat(c);
    }
    getBody(&httpResponse);
    #ifdef DEBUG
      Serial.println("Body of response: ");
      Serial.println(httpResponse);
      Serial.println("End of body");
    #endif
  } else {
    #ifdef DEBUG
      Serial.println("connection failed, try again");
    #endif
  }
  client.stop();
  return httpResponse;
}

States NodeJS::parseHTTPReply(String http){
  timer.disableTimerInterrupt();
  States nextState = NOTHING;
  if(http.startsWith("POLL_ACK_GO_SCAN")){
    nextState = BT_CONFIGURE_SCAN;
  }else if(http.startsWith("POLL_ACK_GO_BROADCAST")){
    nextState = BT_CONFIGURE_ADVERTISE;
  }else if(http.startsWith("POLL_ACK_NOTHING")){
    nextState = NOTHING;
  }else if(http.startsWith("POLL_ACK_SCANNING")){
    nextState = BT_CONFIGURE_SCAN;
  }else if(http.startsWith("POLL_ACK_BROADCASTING")){
    nextState = BT_ADVERTISING;
  }else if(http.startsWith("POLL_ACK_GO_RESET")){
    nextState = RESET;
  }
  timer.enableTimerInterrupt();
  return nextState;
}

States NodeJS::getState(){
  return state;
}

void NodeJS::setState(States s){
  state = s;
}

States NodeJS::getPreviousState(){
  return previousState;
}

void NodeJS::setPreviousState(States s){
  previousState = s;
}

String NodeJS::getHTTPReply(){
  return HTTPReply;
}

void NodeJS::setHTTPReply(String H){
  HTTPReply = H;
}
