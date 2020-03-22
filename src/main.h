/*
 main.h - declarations for main.c

 TODO: decouple functions into separate files/classes
 */

#undef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SimpleTimer.h>
#include <DHTesp.h>

#include "scheduler.h"
#include "grower_version.h"
#include "PowerManager.h"
#include "Device.h"
#include "WaterDevice.h"
#include "WebServer.h"
#include "endpoints.h"

/***************************/
/*        FUNCTIONS        */
/***************************/
void initServer();

void sendReply(const String& content);

// function gets and uses temperature and relative humidity (RH) data
void tempRhDataHandler();

void lampStatus();

void ensureBlynkConnection();

/**************************/
/*     INIT FUNCTIONS     */
/**************************/
void initWiFi(String SSID, String PSK);

void initBlynk();

void initDHT();

void initLamp();

/*****************************/
/*     UTILITY FUNCTIONS     */
/*****************************/
void getTempRh();

uint16_t getLightValue();


void scheduledLampPowerOn();

void scheduledLampPowerOff();

void scheduledFanPowerOn();

void scheduledFanPowerOff();

void scheduledHumPowerOn();

void scheduledHumPowerOff();

void systemRestart();

void sendTempRhToBlynk();

void sendLampToBlynk(uint8_t brightness);
