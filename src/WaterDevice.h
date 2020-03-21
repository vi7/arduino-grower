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
#include <schedules.h>
#include <scheduler.h>


class WaterDevice {
    public:
        static SimpleTimer timer;
        static Scheduler scheduler;

        static void init();

        static void water();

        static void pumpOn();

        static void pumpOff();

};

#endif