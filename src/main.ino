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
#include "Adafruit_Sensor.h"
#include "Adafruit_LSM303_U.h"
#include "SparkFun_Si7021_Breakout_Library.h"
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
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);
M24SR m24sr;
Weather weather;

EthernetUDP Udp;
unsigned int localPort = 8888;                                // local port to listen for UDP packets

String URL = "http://10.128.69.169:3000/id";

// Setup
void setup(){
  // Bluetooth test
  bluetooth.init();
  bluetooth.reset();
  bluetooth.getMACInit();
  bluetooth.setupScan();

  // Read dipswitch
  Serial.print("Dipswitch: ");
  Serial.println(pins.readDipswitch(), DEC);

  // Barometer test
  Serial.println(F("Barometer test"));
  if (! baro.begin()) {
      Serial.println(F("Couldnt find sensor"));
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

  // Magnetometer & accelerometer test
  if(!mag.begin() || !accel.begin()){
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println(F("Ooops, no LSM303 detected ... Check your wiring!"));
    while(1);
  }

  /* Enable auto-gain */
  // Mag
  mag.enableAutoRange(true);
  sensor_t sensor_mag;
  mag.getSensor(&sensor_mag);
  Serial.println(F("------------------------------------"));
  Serial.print  (F("Sensor:       ")); Serial.println(sensor_mag.name);
  Serial.print  (F("Driver Ver:   ")); Serial.println(sensor_mag.version);
  Serial.print  (F("Unique ID:    ")); Serial.println(sensor_mag.sensor_id);
  Serial.print  (F("Max Value:    ")); Serial.print(sensor_mag.max_value); Serial.println(F(" uT"));
  Serial.print  (F("Min Value:    ")); Serial.print(sensor_mag.min_value); Serial.println(F(" uT"));
  Serial.print  (F("Resolution:   ")); Serial.print(sensor_mag.resolution); Serial.println(F(" uT"));
  Serial.println(F("------------------------------------"));
  Serial.println(F(""));
  // Accelerormeter
  sensor_t sensor_accel;
  accel.getSensor(&sensor_accel);
  Serial.println(F("------------------------------------"));
  Serial.print  (F("Sensor:       ")); Serial.println(sensor_accel.name);
  Serial.print  (F("Driver Ver:   ")); Serial.println(sensor_accel.version);
  Serial.print  (F("Unique ID:    ")); Serial.println(sensor_accel.sensor_id);
  Serial.print  (F("Max Value:    ")); Serial.print(sensor_accel.max_value); Serial.println(F(" m/s^2"));
  Serial.print  (F("Min Value:    ")); Serial.print(sensor_accel.min_value); Serial.println(F(" m/s^2"));
  Serial.print  (F("Resolution:   ")); Serial.print(sensor_accel.resolution); Serial.println(F(" m/s^2"));
  Serial.println(F("------------------------------------"));
  Serial.println(F(""));
  delay(250);

  // Si7021
  weather.begin();
  float humidity = 0;
  float tempf = 0;
  humidity = weather.getRH();
  tempf = weather.getTempF();
  Serial.print("Temp:");
  Serial.print(tempf);
  Serial.print("F, ");
  Serial.print("Humidity:");
  Serial.print(humidity);
  Serial.println("%");
  delay(250);

  // M24SR test
  m24sr._setup();
  displayFreeRAM();
  NdefMessage message = NdefMessage();
  message.addUriRecord(&URL[0]);
  displayFreeRAM();
  m24sr.writeNdefMessage(&message);
  delay(250);

  // NodeJS test
  nodejs.NodeJSConnect(bluetooth.getMAC());
  delay(250);

  // RTC test
  Udp.begin(localPort);
  Serial.println("\nWaiting for sync");
  RTC.set(getNtpTime(Udp));
  setSyncProvider(RTC.get);
  digitalClockDisplay();

  delay(5000);
}

// Loop
void loop(){
  //  Magnetometer & accelerometer readout
  Serial.println("MANGETO&ACCELERO");
  // Get a new sensor event
  sensors_event_t event;
  accel.getEvent(&event);
  /* Display the results (acceleration is measured in m/s^2) */
  Serial.print("X: "); Serial.print(event.acceleration.x); Serial.print("  ");
  Serial.print("Y: "); Serial.print(event.acceleration.y); Serial.print("  ");
  Serial.print("Z: "); Serial.print(event.acceleration.z); Serial.print("  ");Serial.println("m/s^2 ");
  mag.getEvent(&event);
  Serial.print("X: "); Serial.print(event.magnetic.x); Serial.print("  ");
  Serial.print("Y: "); Serial.print(event.magnetic.y); Serial.print("  ");
  Serial.print("Z: "); Serial.print(event.magnetic.z); Serial.print("  ");Serial.println("uT");
  delay(500);
  Serial.println();

  // Barometer
  Serial.println(F("BAROMETER"));
  float pascals = baro.getPressure();
  // Our weather page presents pressure in Inches (Hg)
  // Use http://www.onlineconversion.com/pressure.htm for other units
  Serial.print(pascals/3377); Serial.println(F(" Inches (Hg)"));
  float altm = baro.getAltitude();
  Serial.print(altm); Serial.println(F(" meters"));
  float tempC = baro.getTemperature();
  Serial.print(tempC); Serial.println(F("*C"));
  delay(500);
  Serial.println();

  // Weather
  Serial.println(F("WEATHER"));
  float humidity = 0;
  float tempf = 0;
  humidity = weather.getRH();
  tempf = weather.getTempF();
  Serial.print("Temp:");
  Serial.print((tempf-32)*(5.0/9.0));
  Serial.print("*C, ");
  Serial.print("Humidity:");
  Serial.print(humidity);
  Serial.println("%");
  delay(500);
  Serial.println();

  // Print clock (RTC)
  Serial.println(F("TIME"));
  digitalClockDisplay();
  delay(500);
}
