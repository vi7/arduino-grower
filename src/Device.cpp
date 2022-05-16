/**
 * Device.cpp
 **/

#include "Device.h"

void Device::powerOn() {
    #ifdef RADIO_POWER
    this->isPowerOn = PowerManager::manualPowerOn(&_onCode);
    #else
    this->isPowerOn = PowerManager::manualPowerOn(_pin);
    #endif
    this->isAutoPowerOn = true;
}

void Device::powerOff() {
    #ifdef RADIO_POWER
    this->isPowerOn = PowerManager::manualPowerOff(&_offCode);
    #else
    this->isPowerOn = PowerManager::manualPowerOff(_pin);
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