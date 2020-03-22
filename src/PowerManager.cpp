/**
 *  PowerManager.cpp
 */ 


#include "PowerManager.h"


void PowerManager::autoPower(bool *autoControl, bool *isOn, float *currVal, float maxVal, float valHyst, uint8_t pin, WidgetLED *led) {
  if (!*autoControl) return;
  if (*currVal >= maxVal && *isOn) {
    *isOn = commonPower(pin, false, led);
  }
  else if (*currVal < maxVal - valHyst && !*isOn) {
    *isOn = commonPower(pin, true, led);
  }
}

bool PowerManager::commonPower(uint8_t pin, bool enabled, WidgetLED *led) {
  bool isOn;
  if (enabled) {
    digitalWrite(pin, RELAY_ON);
    isOn = true;
    Serial.println("Blynk: enabling led for pin: " + String(pin));
    led->on();
  } else {
    digitalWrite(pin, RELAY_OFF);
    isOn = false;
    Serial.println("Blynk: disabling led for pin: " + String(pin));
    led->off();
  }
  return isOn;
}

// probably redundant
bool PowerManager::manualPower(bool enabled, uint8_t pin, WidgetLED *led) {
    return commonPower(pin, enabled, led);
}
