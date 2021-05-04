/**
 * Device.cpp
 **/

#include "Device.h"

void Device::powerOn() {
    this->isPowerOn = PowerManager::manualPowerOn(pin);
    this->isAutoPowerOn = true;
}

void Device::powerOff() {
    this->isPowerOn = PowerManager::manualPowerOff(pin);
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