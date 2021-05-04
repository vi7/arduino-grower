/**
 * Device.h
 **/

#ifndef DEVICE_H
#define DEVICE_H

#include <Arduino.h>
#include "PowerManager.h"
#include "scheduler.h"

class Device {

    public:
        uint8_t pin; 
        bool isPowerOn;
        bool isAutoPowerOn;

        Device(){};

        Device(uint8_t pin):
         pin(pin),
         isAutoPowerOn(true) {
            pinMode(pin, OUTPUT);
            this->isPowerOn = PowerManager::manualPowerOn(pin);
        };

        void powerOn();

        void powerOff();

        void scheduledPowerOn(Scheduler scheduler);

        void scheduledPowerOff(Scheduler scheduler);

        virtual String status();

};

#endif