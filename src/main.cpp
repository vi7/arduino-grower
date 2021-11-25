/*
 * main.cpp
 *
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SimpleTimer.h>

#include "scheduler.h"
#include "schedules.h"
#include "grower_version.h"
#include "WebServer.h"
#include "endpoints.h"
#include "Device.h"
#include "WaterDevice.h"
/*
 * DHT22 sensor is replaced by the HTU21D sensor
 * related DHTDevice class left for reference purposes
 * TODO: remove DHTDevice class completely after HTU21D prod tests
 */
// #include "DHTDevice.h"
#include "HTU2xDDevice.h"
#include "LDRDevice.h"
#include "MetricsExporter.h"
#include "MetricsCollectable.h"

/*
 * include secrets and credentials
 * file should be created manually on a basis of "secrets.h.example"
 * it'll be ignored by git
 */
#include "secrets.h"

/*
 * Device pins and wire colors
 *
 * !!! DO NOT USE pins D1(5),D2(4)
 * because they are reserved
 * for I2C bus SCL,SDA !!!
 */
#define FANRELAYPIN D0   // white
#define LAMPRELAYPIN D3  // white/light-brown
#define HUMRELAYPIN D4   // violet
#define PUMPPIN D5       // blue
#define LDRPIN A0        // dark-brown/blue

/* monitoring constants */
const uint8_t MAX_TEMP = 40;
const uint8_t TEMP_HYSTERESIS = 10;
// Recommended RH values:
// - vegetative - 60%
// - flowering - 50%
const uint8_t MAX_RH = 60;
const uint8_t RH_HYSTERESIS = 10;
// lamp check interval in seconds
const uint8_t LIGHT_CHECK_INTERVAL = 1;

SimpleTimer timer;
WebServer server(80);

Device fan(FANRELAYPIN);
Device hum(HUMRELAYPIN);
Device lamp(LAMPRELAYPIN);
WaterDevice waterDevice(PUMPPIN);
HTU2xDDevice htu2xD;  // HTU21D temperature and humidity sensor
LDRDevice ldr(LDRPIN);  // Light-Dependent Resistor (photoresistor) - light sensor

Scheduler waterScheduler;
Scheduler lampOnScheduler, lampOffScheduler;
Scheduler fanOnScheduler, fanOffScheduler;
Scheduler humOnScheduler, humOffScheduler;

MetricsExporter htu2xDDeviceExporter((ESP8266WebServer*)&server, (MetricsCollectable*)&htu2xD);


/**************************/
/*     FUNCTIONS     */
/**************************/
void initWiFi(String SSID, String PSK) {
  WiFi.begin(SSID, PSK);

  Serial.print("Connecting to: " + SSID);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println(F("WiFi connected"));
  Serial.print(F("MAC: "));
  Serial.println(WiFi.macAddress());
  Serial.print(F("IP address: "));
  Serial.println(WiFi.localIP());
}

void systemRestart() {
  Serial.println(F("Restarting device..."));
  ESP.restart();
}

/*****************/
/*     SETUP     */
/*****************/
void setup() {

  // init Serial for logging and say Hi
  Serial.begin(115200);
  const char BOARD_IDENTITY[] = ARDUINO_BOARD;
  Serial.println(F("\n\n************"));
  Serial.printf("%s", BOARD_IDENTITY);
  Serial.println(F(" is up. Hey there!"));
  Serial.printf("Firmware version: %s\n", GROWER_VERSION);
  Serial.println(F("************\n\n"));

  initWiFi(WIFI_SSID, WIFI_PSK);

  /* SimpleTimer function execution scheduling */
  timer.setInterval(htu2xD.getReadInterval(), []{htu2xD.tempDataHandler(&lamp, MAX_TEMP, TEMP_HYSTERESIS);});
  timer.setInterval(htu2xD.getReadInterval(), []{htu2xD.rhDataHandler(&hum, MAX_RH, RH_HYSTERESIS);});
  timer.setInterval(LIGHT_CHECK_INTERVAL * 1000, []{ldr.lampStatus();});

  // TODO: candidate for debug logging
  // Serial.print(F("[MAIN] [D] Requested watering start time: "));
  // Serial.println(waterScheduler.getStartDateTime());
  // Serial.print(F("[MAIN] [D] Next watering scheduled on: "));
  // Serial.println(waterScheduler.getNextDateTime());

  waterScheduler = Scheduler([]{
    waterDevice.scheduledWater(
      waterScheduler, []{waterDevice.powerOff();}
    );
  }, WATER_SCHEDULE);
  lampOnScheduler = Scheduler([]{lamp.scheduledPowerOn(lampOnScheduler);}, LAMP_ON_SCHEDULE);
  lampOffScheduler = Scheduler([]{lamp.scheduledPowerOff(lampOffScheduler);}, LAMP_OFF_SCHEDULE);
  fanOnScheduler = Scheduler([]{fan.scheduledPowerOn(fanOnScheduler);}, FAN_ON_SCHEDULE);
  fanOffScheduler = Scheduler([]{fan.scheduledPowerOff(fanOffScheduler);}, FAN_OFF_SCHEDULE);
  humOnScheduler = Scheduler([]{hum.scheduledPowerOn(humOnScheduler);}, HUM_ON_SCHEDULE);
  humOffScheduler = Scheduler([]{hum.scheduledPowerOff(humOffScheduler);}, HUM_OFF_SCHEDULE);

  server.registerEndpoint(&lamp, LAMP_ENDPOINTS, sizeof(LAMP_ENDPOINTS)/sizeof(LAMP_ENDPOINTS[0]));
  server.registerEndpoint(&fan, FAN_ENDPOINTS, sizeof(FAN_ENDPOINTS)/sizeof(FAN_ENDPOINTS[0]));
  server.registerEndpoint(&hum, HUM_ENDPOINTS, sizeof(HUM_ENDPOINTS)/sizeof(HUM_ENDPOINTS[0]));
  server.registerEndpoint(&waterDevice, WATER_ENDPOINTS, sizeof(WATER_ENDPOINTS)/sizeof(WATER_ENDPOINTS[0]));
  // TODO: rename DHT endpoints to HTU2xD endpoints
  server.registerEndpoint(&htu2xD, DHT_ENDPOINTS, sizeof(DHT_ENDPOINTS)/sizeof(DHT_ENDPOINTS[0]));
  server.registerEndpoint(&ldr, LDR_ENDPOINTS, sizeof(LDR_ENDPOINTS)/sizeof(LDR_ENDPOINTS[0]));

  server.begin();
}

/****************/
/*     LOOP     */
/****************/
void loop() {

  timer.run();
  ezt::events();
  server.handleClient();
}