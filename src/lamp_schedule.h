/*
* 
* lamp_schedule.h
*
* Defines constants for scheduling lamp turning on and off
*
*/

#ifndef LAMP_SCHEDULE_H
#define LAMP_SCHEDULE_H

#include <Arduino.h>

const schedule LAMP_ON_SCHEDULE = {
  0,    // sec
  0,    // min
  18,   // hour
  8,    // day
  3,    // month
  2020, // year
  1,    // interval in days
};

const schedule LAMP_OFF_SCHEDULE = {
  0,    // sec
  0,    // min
  14,   // hour
  8,    // day
  3,    // month
  2020, // year
  1,    // interval in days
};

#endif