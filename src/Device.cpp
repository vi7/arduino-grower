/**
 * Device.cpp
 **/

#include "Device.h"

Device::Device() {
};

void Device::init(uint8_t pin, uint8_t blynkPin) {
    this->pin = pin;
    this->led = new WidgetLED(blynkPin);

    pinMode(pin, OUTPUT);

    isPowerOn = PowerManager::manualPower(true, pin, led);
    isAutoPowerOn = true;
};

void Device::powerOn() {
    isPowerOn = PowerManager::manualPower(true, pin, led);
    isAutoPowerOn = true;
}

void Device::powerOff() {
    this->isPowerOn = PowerManager::manualPower(false, pin, led);
    isAutoPowerOn = false;
}

void Device::scheduledPowerOn(Scheduler scheduler) {
    powerOn();
    scheduler.setNextEvent();
}

void Device::scheduledPowerOff(Scheduler scheduler) {
    powerOff();
    scheduler.setNextEvent();
}

String Device::status() {
    return "{\"power\":\"" + String(isPowerOn) +"\"}";
}