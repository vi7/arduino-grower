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

#include "scheduler.h"
#include "grower_version.h"
#include "PowerManager.h"
#include "Device.h"
#include "WaterDevice.h"
#include "DHTDevice.h"
#include "WebServer.h"
#include "endpoints.h"

/***************************/
/*        FUNCTIONS        */
/***************************/
void lampStatus();

/**************************/
/*     INIT FUNCTIONS     */
/**************************/
void initWiFi(String SSID, String PSK);

void initLamp();

/*****************************/
/*     UTILITY FUNCTIONS     */
/*****************************/
uint16_t getLightValue();

void systemRestart();

void sendLampToBlynk(uint8_t brightness);
