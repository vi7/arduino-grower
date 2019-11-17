/*
 main.h - declarations for main.c

 TODO: decouple functions into separate files/classes
 */

#undef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266_SSL.h>
#include <SimpleTimer.h>
#include <DHTesp.h>

/***************************/
/*        FUNCTIONS        */
/***************************/
void sendResponse(WiFiClient client);

// function gets and uses temperature and relative humidity (RH) data
void tempRhDataHandler();

void water();

void lampStatus();

void ensureBlynkConnection();

/**************************/
/*     INIT FUNCTIONS     */
/**************************/
void initWiFi(String SSID, String PSK);

void initBlynk();

void initDHT();

void initLampRelay();

void initFanRelay();

void initHumRelay();

void initPump();

void initLamp();

/*****************************/
/*     UTILITY FUNCTIONS     */
/*****************************/
void getTempRh();

uint16_t getLightValue();

// function performs actions when temperature exceeds MAX_TEMP limit
void checkTemp();

void powerOff(bool yes);

bool commonPower(uint8_t pin, bool enabled, WidgetLED *led);

void manualLampPowerOn();

void manualLampPowerOff();

void manualFanPower(bool enabled);

void manualHumPower(bool enabled);

void pumpOn();

void pumpOff();

void systemRestart();

void sendTempRhToBlynk();

void sendLampToBlynk(bool isOn, uint8_t brightness);
