/**
 * LDRDevice.cpp
 **/

#include "LDRDevice.h"

void LDRDevice::init(uint8_t pin, uint8_t blynkPin) {
  this->pin = pin;
  this->blynkPin = blynkPin;
  this->led = new WidgetLED(blynkPin);
  uint16_t lightVal = getLightValue();
  this->isPowerOn = lightVal < LAMP_ON_VALUE ? true : false;
}

uint16_t LDRDevice::getLightValue() {
  return analogRead(pin);
}

void LDRDevice::lampStatus() {
  uint16_t lightVal = getLightValue();
  uint8_t ledBrightness = map(lightVal, 0, 1023, 255, 0);
  // TODO: candidate for debug logging
  // Serial.println("LDR sensor: " + String(lightVal));
  if (lightVal < LAMP_ON_VALUE && !isPowerOn) {
    Serial.println(F("LDR sensor: lamp is on"));
    isPowerOn = true;
  } else if (lightVal > LAMP_OFF_VALUE && isPowerOn) {
    Serial.println(F("LDR sensor: lamp is off"));
    isPowerOn = false;
  }
  BlynkManager::sendLampToBlynk(ledBrightness, led);
}


String LDRDevice::status() {
    uint8_t ledBrightness = map(getLightValue(), 0, 1023, 255, 0);
    return "{\"brightness\":\"" + String(ledBrightness) +"\"}";
}