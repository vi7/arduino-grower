/*
 * main.cpp
 *
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SimpleTimer.h>
#include <RCSwitch.h>

#include "Settings.h"
/*
 * include secrets and credentials
 * file should be created manually on a basis of "secrets.h.example"
 * it'll be ignored by git
 */
#include "secrets.h"

#include "scheduler.h"
#include "schedules.h"
#include "version.h"
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

SimpleTimer timer;
WebServer server(WEB_SERVER_PORT);

#ifdef RADIO_POWER
RCSwitch PowerManager::transmitter = RCSwitch();
Device outFan(OUTFAN_ON_CODE, OUTFAN_OFF_CODE);
Device hum(HUM_ON_CODE, HUM_OFF_CODE);
/*
 * Additional 'humMains' device switches on an outlet on main power
 * extender where we have humidifier power outlet connected
 */
Device humMains(HUM_MAINS_ON_CODE, HUM_MAINS_OFF_CODE);
Device lamp(LAMP_ON_CODE, LAMP_OFF_CODE);
Device fan(FAN_ON_CODE, FAN_OFF_CODE);
#else
Device hum(HUMRELAYPIN);
Device lamp(LAMPRELAYPIN);
Device fan(FANRELAYPIN);
#endif
WaterDevice waterDevice(PUMPPIN);
HTU2xDDevice htu2xD;  // HTU21D temperature and humidity sensor
LDRDevice ldr(LDRPIN);  // Light-Dependent Resistor (photoresistor) - light sensor

Scheduler waterScheduler;
Scheduler lampOnScheduler, lampOffScheduler;
Scheduler fanOnScheduler, fanOffScheduler;
// Disable Humidifier scheduler until we have separate on and off code again (or forever?)
// Scheduler humOnScheduler, humOffScheduler;

MetricsExporter htu2xDDeviceExporter((ESP8266WebServer*)&server, (MetricsCollectable*)&htu2xD);

/**************************/
/*     FUNCTIONS     */
/**************************/
void initWiFi(String SSID, String PSK) {
  IPAddress ipAddress;
  IPAddress gateway;
  IPAddress subnet;
  IPAddress dns1;
  IPAddress dns2;

  ipAddress.fromString(IP_ADDRESS);
  gateway.fromString(GATEWAY);
  subnet.fromString(SUBNET);
  dns1.fromString(DNS1);
  dns2.fromString(DNS2);

  WiFi.disconnect();
  Serial.println(F("Hostname: ") + String(HOSTNAME));
  WiFi.hostname(HOSTNAME);
  WiFi.config(ipAddress, gateway, subnet, dns1, dns2);
  WiFi.begin(SSID, PSK);
  WiFi.enableSTA(true);
  Serial.print(F("MAC: "));
  Serial.println(WiFi.macAddress());

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

void initRadio() {
  PowerManager::transmitter.enableTransmit(TX_PIN);
  PowerManager::transmitter.setProtocol(RCSWITCH_PROTOCOL);
  Serial.println(F("Radio transmission initialized"));
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
  Serial.printf("Firmware version: %s\n", VERSION);
  Serial.println(F("************\n"));

  initWiFi(WIFI_SSID, WIFI_PSK);
  #ifdef RADIO_POWER
  initRadio();
  #endif

  /* SimpleTimer function execution scheduling */
  timer.setInterval(htu2xD.getReadInterval(), []{htu2xD.tempDataHandler(&lamp, MAX_TEMP, TEMP_HYSTERESIS);});
  timer.setInterval(180000, []{htu2xD.rhDataHandler(&hum, MAX_RH, RH_HYSTERESIS);});
  timer.setInterval(LIGHT_CHECK_INTERVAL * 1000, []{ldr.lampStatus();});

  outFan.powerOn();
  humMains.powerOn();
  waterScheduler = Scheduler([]{
    waterDevice.scheduledWater(
      waterScheduler, []{waterDevice.powerOff();}
    );
  }, WATER_SCHEDULE);
  lampOnScheduler = Scheduler([]{lamp.scheduledPowerOn(lampOnScheduler);}, LAMP_ON_SCHEDULE);
  lampOffScheduler = Scheduler([]{lamp.scheduledPowerOff(lampOffScheduler);}, LAMP_OFF_SCHEDULE);
  /* Uncomment scheduler lines below to enable automatic on/off for the fan */
  // fanOnScheduler = Scheduler([]{fan.scheduledPowerOn(fanOnScheduler);}, FAN_ON_SCHEDULE);
  // fanOffScheduler = Scheduler([]{fan.scheduledPowerOff(fanOffScheduler);}, FAN_OFF_SCHEDULE);
  fan.powerOn();
  // Disable Humidifier scheduler until we have separate on and off code again (or forever?)
  // humOnScheduler = Scheduler([]{hum.scheduledPowerOn(humOnScheduler);}, HUM_ON_SCHEDULE);
  // humOffScheduler = Scheduler([]{hum.scheduledPowerOff(humOffScheduler);}, HUM_OFF_SCHEDULE);

  server.registerEndpoint(&lamp, LAMP_ENDPOINTS, sizeof(LAMP_ENDPOINTS)/sizeof(LAMP_ENDPOINTS[0]));
  server.registerEndpoint(&fan, FAN_ENDPOINTS, sizeof(FAN_ENDPOINTS)/sizeof(FAN_ENDPOINTS[0]));
  server.registerEndpoint(&hum, HUM_ENDPOINTS, sizeof(HUM_ENDPOINTS)/sizeof(HUM_ENDPOINTS[0]));
  server.registerEndpoint(&waterDevice, WATER_ENDPOINTS, sizeof(WATER_ENDPOINTS)/sizeof(WATER_ENDPOINTS[0]));
  server.registerEndpoint(&htu2xD, HT_ENDPOINTS, sizeof(HT_ENDPOINTS)/sizeof(HT_ENDPOINTS[0]));
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