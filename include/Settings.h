/**
 *
 * Settings.h
 *
 **/

/*
 * !! WARNING !!
 *
 * !!! DO NOT USE pins 5(D1),4(D2)
 * because they are reserved
 * for I2C bus SCL,SDA !!!
 *
 * !! WARNING !!
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>

#ifndef CUSTOM_NETWORK
#define IP_ADDRESS  "192.168.1.30"
#define SUBNET      "255.255.255.0"
#define GATEWAY     "192.168.1.1"
#define DNS1        "192.168.1.4"
#define DNS2        "192.168.1.1"
#define HOSTNAME    "arduino-grower"
#else
#define IP_ADDRESS  "192.168.1.31"
#define SUBNET      "255.255.255.0"
#define GATEWAY     "192.168.1.1"
#define DNS1        "192.168.1.4"
#define DNS2        "192.168.1.1"
#define HOSTNAME    "test-arduino-grower"
#endif

#define WEB_SERVER_PORT 80
#define RADIO_POWER  // Enable power control over radio transmission

#ifdef RADIO_POWER
  #define TX_PIN 2  // D4 - orange
  #define RCSWITCH_PROTOCOL 13
  /*
   * ORNO OR-AE-13132(GS) power extender codes
   */
  #define MSG_LENGTH 24

  #define OUTLET1_ON  0x15533
  #define OUTLET1_OFF 0x1553C
  #define OUTLET2_ON  0x155C3
  #define OUTLET2_OFF 0x155CC
  #define OUTLET3_ON  0x15703
  #define OUTLET3_OFF 0x1570C
  #define OUTLET4_ON  0x15D03
  #define OUTLET4_OFF 0x15D0C
  #define OUTLET5_ON  0x17503
  #define OUTLET5_OFF 0x1750C
  #define SSR_OUTLET_ON   0x17703
  #define SSR_OUTLET_OFF  0x1770C

  #define OUTFAN_ON_CODE      OUTLET1_ON
  #define OUTFAN_OFF_CODE     OUTLET1_OFF
  #define HUM_ON_CODE         SSR_OUTLET_ON
  #define HUM_OFF_CODE        SSR_OUTLET_OFF
  #define HUM_MAINS_ON_CODE   OUTLET2_ON
  #define HUM_MAINS_OFF_CODE  OUTLET2_OFF
  #define FAN_ON_CODE         OUTLET3_ON
  #define FAN_OFF_CODE        OUTLET3_OFF
  #define LAMP_ON_CODE        OUTLET4_ON
  #define LAMP_OFF_CODE       OUTLET4_OFF
#else
  #define LAMPRELAYPIN  0  // white/light-brown
  #define HUMRELAYPIN   2   // violet
  #define FANRELAYPIN   16   // white
#endif

#define PUMPPIN 14       // D5 - blue
#define LDRPIN A0        // dark-brown/blue

// TODO: replace const's with define's
/* monitoring constants */
const uint8_t MAX_TEMP = 40;
const uint8_t TEMP_HYSTERESIS = 10;
// Recommended RH values:
// - vegetative - 60%
// - flowering - 50%
const uint8_t MAX_RH = 50;
const uint8_t RH_HYSTERESIS = 20;
// lamp check interval in seconds
const uint8_t LIGHT_CHECK_INTERVAL = 10;

#endif