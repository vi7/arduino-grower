/**
 * Device.h
 **/

#ifndef DEVICE_H
#define DEVICE_H

#include <Arduino.h>
#include <BlynkSimpleEsp8266_SSL.h>
#include "PowerManager.h"
#include "scheduler.h"

class Device {

    public:

        Device();

        uint8_t pin; 
        uint8_t blynkPin;
        WidgetLED *led;
        bool isPowerOn;
        bool isAutoPowerOn;

        void init(uint8_t pin, uint8_t blynkPin);

        void powerOn();

        void powerOff();

        void scheduledPowerOn(Scheduler scheduler);

        void scheduledPowerOff(Scheduler scheduler);

        virtual String status();

};

#endif