/**
 * LDRDevice.cpp
 **/

#include "LDRDevice.h"

uint16_t LDRDevice::getLightValue() {
  return analogRead(pin);
}

void LDRDevice::lampStatus() {
  uint16_t lightVal = getLightValue();
  // TODO: candidate for debug logging
  // Serial.println("LDR sensor: " + String(lightVal));
  if (lightVal < LAMP_ON_VALUE && !isPowerOn) {
    Serial.println(F("LDR sensor: lamp is on"));
    isPowerOn = true;
  } else if (lightVal > LAMP_OFF_VALUE && isPowerOn) {
    Serial.println(F("LDR sensor: lamp is off"));
    isPowerOn = false;
  }
}


String LDRDevice::status() {
    uint8_t ledBrightness = map(getLightValue(), 0, 1023, 255, 0);
    return "{\"brightness\":\"" + String(ledBrightness) +"\"}";
}