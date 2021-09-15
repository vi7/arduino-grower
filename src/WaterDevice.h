/**
 * WaterDevice.h
 **/

#ifndef WATERDEVICE_H
#define WATERDEVICE_H

#define PUMP_ON HIGH
#define PUMP_OFF LOW

#include <Arduino.h>
#include <SimpleTimer.h>
#include <scheduler.h>
#include <schedules.h>
#include "Device.h"

extern SimpleTimer timer;

class WaterDevice: public Device {
    public:

        WaterDevice(uint8_t pin) {
            this->_pin = pin;
            pinMode(pin, OUTPUT);
            digitalWrite(pin, PUMP_OFF);
        };

        void powerOn();

        void powerOff();

        void water(timer_callback pumpOffFunc);
        
        void scheduledWater(Scheduler scheduler, timer_callback pumpOffFunc);

        String status();

};

#endif