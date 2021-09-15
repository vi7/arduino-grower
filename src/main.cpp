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
#include "DHTDevice.h"
#include "LDRDevice.h"
#include "MetricsExporter.h"
#include "MetricsCollectable.h"

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
DHTDevice dht(DHTPIN);
LDRDevice ldr(LDRPIN);

Scheduler waterScheduler;
Scheduler lampOnScheduler, lampOffScheduler;
Scheduler fanOnScheduler, fanOffScheduler;
Scheduler humOnScheduler, humOffScheduler;

MetricsExporter dhtDeviceExporter((ESP8266WebServer*)&server, (MetricsCollectable*)&dht);


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

  /* SimpleTimer function execution scheduling */
  timer.setInterval(dht.dhtReadInterval, []{dht.tempDataHandler(&lamp, MAX_TEMP, TEMP_HYSTERESIS);});
  timer.setInterval(dht.dhtReadInterval, []{dht.rhDataHandler(&hum, MAX_RH, RH_HYSTERESIS);});
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
  server.registerEndpoint(&dht, DHT_ENDPOINTS, sizeof(DHT_ENDPOINTS)/sizeof(DHT_ENDPOINTS[0]));
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