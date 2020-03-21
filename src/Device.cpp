/**
 * Device.cpp
 **/

#include "Device.h"

Device::Device() {
};

void Device::init(uint8_t pin, WidgetLED *led) {
    this->led = led;
    this->pin = pin;

    pinMode(pin, OUTPUT);
    PowerManager::manualPower(true, pin, led, isPowerOn, isAutoPowerOn);
};

