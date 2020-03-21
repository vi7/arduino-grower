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

static SimpleTimer waterTimer;
static Scheduler scheduler;

class WaterDevice {
    public:
        static void init();

        static void water();

        static void pumpOn();

        static void pumpOff();

};

#endif