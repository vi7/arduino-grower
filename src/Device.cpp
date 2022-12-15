/**
 * Device.cpp
 **/

#include "Device.h"

void Device::powerOn() {
    #ifdef RADIO_POWER
    this->isPowerOn = PowerManager::manualPowerOn(&_onCode);
    Serial.printf("Device powered on with code %X\n", _onCode);
    #else
    this->isPowerOn = PowerManager::manualPowerOn(_pin);
    Serial.printf("Device powered on on pin %d\n", _pin);
    #endif
    this->isAutoPowerOn = true;
}

void Device::powerOff() {
    #ifdef RADIO_POWER
    this->isPowerOn = PowerManager::manualPowerOff(&_offCode);
    Serial.printf("Device powered off with code %X\n", _offCode);
    #else
    this->isPowerOn = PowerManager::manualPowerOff(_pin);
    Serial.printf("Device powered off on pin %d\n", _pin);
    #endif
    this->isAutoPowerOn = false;
}

void Device::scheduledPowerOn(Scheduler scheduler) {
    this->powerOn();
    scheduler.setNextEvent();
}

void Device::scheduledPowerOff(Scheduler scheduler) {
    this->powerOff();
    scheduler.setNextEvent();
}

String Device::status() {
    return "{\"power\":\"" + String(isPowerOn) +"\"}";
}