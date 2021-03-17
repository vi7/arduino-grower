/*
 * main.cpp
 *
 * !!! IMPORTANT !!! don't send more that 10 values per second to Blynk
 *
 */

/* Comment this out to disable Blynk prints and save space: */
//#define BLYNK_PRINT Serial // Defines the object that is used for printing
//#define BLYNK_DEBUG        // Optional, this enables more detailed prints

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
#include "DHTDevice.h"
#include "LDRDevice.h"

/*
 * include secrets and credentials
 * file should be created manually on a basis of "secrets.h.example"
 * it'll be ignored by git
 */
#include "secrets.h"

#define DHTPIN D3
#define LAMPRELAYPIN D1
#define FANRELAYPIN D0
#define HUMRELAYPIN D4
#define LDRPIN A0
#define PUMPPIN D2

/* Blynk defines */
#define BLYNK_LCDPIN V0
#define BLYNK_GRAPHTEMPPIN V1
#define BLYNK_GRAPHRHPIN V2
#define BLYNK_LAMPBRLEDPIN V4
#define BLYNK_LAMPLEDPIN V5
#define BLYNK_GRAPHPUMPPIN V6
#define BLYNK_FANLEDPIN V7
#define BLYNK_HUMLEDPIN V8

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

Device lamp, fan, hum;
WaterDevice waterDevice;
DHTDevice dht;
LDRDevice ldr; 

Scheduler waterScheduler;
Scheduler lampOnScheduler, lampOffScheduler;
Scheduler fanOnScheduler, fanOffScheduler;
Scheduler humOnScheduler, humOffScheduler;


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
  BlynkManager::init(BLYNK_LCDPIN);
  dht.init(DHTPIN, BLYNK_GRAPHTEMPPIN, BLYNK_GRAPHRHPIN);
  ldr.init(LDRPIN, BLYNK_LAMPBRLEDPIN);

  // delay Blynk dependant init functions
  timer.setTimeout(2000, []{lamp.init(LAMPRELAYPIN, BLYNK_LAMPLEDPIN);});
  timer.setTimeout(3000, []{fan.init(FANRELAYPIN, BLYNK_FANLEDPIN);});
  timer.setTimeout(4000, []{hum.init(HUMRELAYPIN, BLYNK_HUMLEDPIN);});
  timer.setTimeout(5000, []{waterDevice.init(PUMPPIN, BLYNK_GRAPHPUMPPIN);});

  /* SimpleTimer function execution scheduling */
  timer.setInterval(dht.dhtReadInterval, []{dht.tempDataHandler(&lamp, MAX_TEMP, TEMP_HYSTERESIS);});
  timer.setInterval(dht.dhtReadInterval, []{dht.rhDataHandler(&hum, MAX_RH, RH_HYSTERESIS);});
  timer.setInterval(LIGHT_CHECK_INTERVAL * 1000, []{ldr.lampStatus();});
  timer.setInterval(BLYNK_CHECK_INTERVAL * 1000, BlynkManager::ensureBlynkConnection);


  lampOnScheduler.init([]{lamp.scheduledPowerOn(lampOnScheduler);}, LAMP_ON_SCHEDULE);
  lampOffScheduler.init([]{lamp.scheduledPowerOff(lampOffScheduler);}, LAMP_OFF_SCHEDULE);
  fanOnScheduler.init([]{fan.scheduledPowerOn(fanOnScheduler);}, FAN_ON_SCHEDULE);
  fanOffScheduler.init([]{fan.scheduledPowerOff(fanOffScheduler);}, FAN_OFF_SCHEDULE);
  humOnScheduler.init([]{hum.scheduledPowerOn(humOnScheduler);}, HUM_ON_SCHEDULE);
  humOffScheduler.init([]{hum.scheduledPowerOff(humOffScheduler);}, HUM_OFF_SCHEDULE);

  // TODO: candidate for debug logging
  // Serial.print(F("[MAIN] [D] Requested watering start time: "));
  // Serial.println(waterScheduler.getStartDateTime());
  // Serial.print(F("[MAIN] [D] Next watering scheduled on: "));
  // Serial.println(waterScheduler.getNextDateTime());

  server.registerEndpoint(&lamp, LAMP_ENDPOINTS, sizeof(LAMP_ENDPOINTS)/sizeof(LAMP_ENDPOINTS[0]));
  server.registerEndpoint(&fan, FAN_ENDPOINTS, sizeof(FAN_ENDPOINTS)/sizeof(FAN_ENDPOINTS[0]));
  server.registerEndpoint(&hum, HUM_ENDPOINTS, sizeof(HUM_ENDPOINTS)/sizeof(HUM_ENDPOINTS[0]));
  server.registerEndpoint(&waterDevice, WATER_ENDPOINTS, sizeof(WATER_ENDPOINTS)/sizeof(WATER_ENDPOINTS[0]));
  server.registerEndpoint(&dht, DHT_ENDPOINTS, sizeof(DHT_ENDPOINTS)/sizeof(DHT_ENDPOINTS[0]));
  server.registerEndpoint(&ldr, LDR_ENDPOINTS, sizeof(LDR_ENDPOINTS)/sizeof(LDR_ENDPOINTS[0]));

  server.begin();

}

/****************/
/*     LOOP     */
/****************/
void loop() {

  Blynk.run();
  timer.run();
  ezt::events();
  server.handleClient();
}

