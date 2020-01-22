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
#include <ezTime.h>

#include "scheduler.h"
#include "grower_version.h"

typedef struct {
  uint8_t sec;
  uint8_t min;
  uint8_t hr;
  uint8_t day;
  uint8_t mnth;
  uint16_t year;
  uint8_t intervalDays;
} schedule;

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

void initEZT();

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

void autoPower(bool *autoControl, bool *isOn, float *currVal, float maxVal, float valHyst, uint8_t pin, WidgetLED *led);

bool commonPower(uint8_t pin, bool enabled, WidgetLED *led);

void manualLampPower(bool enabled);

void manualFanPower(bool enabled);

void manualHumPower(bool enabled);

void pumpOn();

void pumpOff();

void systemRestart();

void sendTempRhToBlynk();

void sendLampToBlynk(uint8_t brightness);
