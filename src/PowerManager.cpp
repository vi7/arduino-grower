/**
 *  PowerManager.cpp
 */


#include "PowerManager.h"
#ifdef RADIO_POWER
void PowerManager::autoPower(bool *autoControl, bool *currentState, float *currVal, float maxVal, float valHyst,
                              uint32_t *onCode, uint32_t *offCode) {
  if (!*autoControl) return;
  if (*currVal >= maxVal) {
    *currentState = manualPowerOff(offCode);
  }
  else if (*currVal < maxVal - valHyst) {
    *currentState = manualPowerOn(onCode);
  }
}

bool PowerManager::manualPowerOn(uint32_t *onCode) {
  PowerManager::transmitter.send(*onCode, MSG_LENGTH);
  return true;
}

bool PowerManager::manualPowerOff(uint32_t *offCode) {
  PowerManager::transmitter.send(*offCode, MSG_LENGTH);
  return false;
}
#else
void PowerManager::autoPower(bool *autoControl, bool *currentState, float *currVal, float maxVal, float valHyst, uint8_t *pin) {
  if (!*autoControl) return;
  if (*currVal >= maxVal) {
    *currentState = manualPowerOff(*pin);
  }
  else if (*currVal < maxVal - valHyst) {
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
#endif
