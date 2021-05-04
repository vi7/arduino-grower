/**
 * WaterDevice.cpp
 **/

#include "WaterDevice.h"

WaterDevice::WaterDevice(uint8_t pin) {
  this->pin = pin;
  pinMode(pin, OUTPUT);
  digitalWrite(pin, PUMP_OFF);
};

void WaterDevice::powerOn() {
  digitalWrite(PUMPPIN, PUMP_ON);
  Serial.println(F("[MAIN] [I] Pump is on"));
}


void WaterDevice::powerOff() {
  digitalWrite(PUMPPIN, PUMP_OFF);
  Serial.println(F("[MAIN] [I] Pump is off"));
}


void WaterDevice::water(timer_callback pumpOffFunc) {
  this->powerOn();
  timer.setTimeout(WATER_DURATION * 1000, pumpOffFunc);
}

void WaterDevice::scheduledWater(Scheduler scheduler, timer_callback pumpOffFunc) {
  this->water(pumpOffFunc);
  scheduler.setNextEvent();
};

String WaterDevice::status() {
// TODO: generate from template using snprintf

  // return "{\"date_time\":\"" + waterScheduler.getStartDateTime(ISO8601) +"\"," 
  // + "\"date_time\":\"" + waterScheduler.getNextDateTime(ISO8601) + "\"," 
  // + "\"days\":\"" + String(WATER_SCHEDULE.intervalDays) +"\","
  // + "\"duration\":\"" + String(WATER_DURATION) +"\"}";
}