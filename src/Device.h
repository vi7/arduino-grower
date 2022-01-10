/**
 * Device.h
 **/

#ifndef DEVICE_H
#define DEVICE_H

#include <Arduino.h>
#include "Settings.h"
#include "PowerManager.h"
#include "scheduler.h"

class Device {

    public:
        bool isPowerOn;
        bool isAutoPowerOn;
        uint8_t _pin;

        Device(){};

        #ifdef RADIO_POWER
        uint32_t _onCode;
        uint32_t _offCode;

        Device(uint32_t onCode, uint32_t offCode):
         isAutoPowerOn(true),
         _onCode(onCode),
         _offCode(offCode) {
            this->isPowerOn = PowerManager::manualPowerOn(&onCode);
        };
        #else
        Device(uint8_t pin):
         _pin(pin),
         isAutoPowerOn(true) {
            pinMode(pin, OUTPUT);
            this->isPowerOn = PowerManager::manualPowerOn(pin);
        };
        #endif

        void powerOn();

        void powerOff();

        void scheduledPowerOn(Scheduler scheduler);

        void scheduledPowerOff(Scheduler scheduler);

        virtual String status();

};

#endif