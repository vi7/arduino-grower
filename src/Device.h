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
        WidgetLED *led;
        bool *isPowerOn;
        bool *isAutoPowerOn;

        void init(uint8_t pin, WidgetLED *led);

        // TODO add scheduler initialization
        // void init(uint8_t pin, WidgetLED *led, schedule scheduleOn, schedule scheduleOff);

        void powerOn();

        void powerOff();

    private:
        // Scheduler schedulerOn;
        // Scheduler schedulerOff;

};


#endif