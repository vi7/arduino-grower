/**
 * WaterDevice.cpp
 **/

#include "WaterDevice.h"

void WaterDevice::powerOn() {
  digitalWrite(_pin, PUMP_ON);
  Serial.println(F("Pump is on"));
}

void WaterDevice::powerOff() {
  digitalWrite(_pin, PUMP_OFF);
  Serial.println(F("Pump is off"));
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