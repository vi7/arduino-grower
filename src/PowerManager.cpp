/**
 *  PowerManager.cpp
 */


#include "PowerManager.h"

void PowerManager::autoPower(bool *autoControl, bool *currentState, float *currVal, float maxVal, float valHyst, uint8_t *pin) {
  if (!*autoControl) return;
  if (*currVal >= maxVal && *currentState) {
    *currentState = manualPowerOff(*pin);
  }
  else if (*currVal < maxVal - valHyst && !*currentState) {
    *currentState = manualPowerOn(*pin);
  }
}

bool PowerManager::manualPowerOn(uint8_t pin) {
  digitalWrite(pin, RELAY_ON);
  return true;
}

bool PowerManager::manualPowerOff(uint8_t pin) {
  digitalWrite(pin, RELAY_OFF);
  return false;
}