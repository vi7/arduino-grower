/**
 * WaterDevice.h
 **/

#ifndef WATERDEVICE_H
#define WATERDEVICE_H

#define PUMP_ON HIGH
#define PUMP_OFF LOW
#define BLYNK_GRAPHPUMPPIN V6
#define PUMPPIN D2


#include <Arduino.h>
#include <SimpleTimer.h>
#include <BlynkSimpleEsp8266_SSL.h>
#include <scheduler.h>
#include <schedules.h>
#include "Device.h"

extern SimpleTimer timer;
extern Scheduler waterScheduler;

// TODO get rid of static, after scheduler init will be moved to main
class WaterDevice: public Device {
    public:

        WaterDevice();

        static void init();

        void init(uint8_t pin, uint8_t blynkPin);

        void powerOn();

        void powerOff();

        String status();

        static void pumpOn();

        static void pumpOff();

        static void water();

    private:
        uint8_t pin;
        uint8_t blynkPin;

};

#endif