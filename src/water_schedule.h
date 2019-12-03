/*
 * water_schedule.h
 *
 * This is a home of watering schedule
 *
 ** Watering solution:
 * Amount of connected pumps: 1
 * Pump type: AD20P-1230C
 * Pump specs:
 *  - 12VDC
 *  - Hmax 300cm
 *  - Qmax 240L/H(66ml/s)
 *  - Qreal 180L/H(50ml/s) - highly depends on the environment like pipe length etc.
 *
 */

#ifndef WATER_SCHEDULE_H
#define WATER_SCHEDULE_H

#include <Arduino.h>

// est water amount: 250ml - time in seconds
const uint8_t WATER_DURATION = 5;
const schedule WATER_SCHEDULE = {
  0,    // sec
  30,    // min
  21,   // hour
  8,    // day
  12,   // month
  2019, // year
  1,    // interval in days
};

#endif
