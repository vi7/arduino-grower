/*
*
* Defines constants for scheduling devices
*
*/

#ifndef SCHEDULES_H
#define SCHEDULES_H

#include <Arduino.h>

// Constants for lamp schedule
const schedule LAMP_ON_SCHEDULE = {
  0,    // sec
  0,    // min
  22,   // hour
  24,    // day
  07,    // month
  2022, // year
  1,    // interval in days
};

const schedule LAMP_OFF_SCHEDULE = {
  0,    // sec
  0,    // min
  16,   // hour
  24,    // day
  07,    // month
  2022, // year
  1,    // interval in days
};

// Constants for fan schedule
const schedule FAN_ON_SCHEDULE = {
  0,    // sec
  1,    // min
  22,   // hour
  27,    // day
  11,    // month
  2021, // year
  1,    // interval in days
};

const schedule FAN_OFF_SCHEDULE = {
  0,    // sec
  1,    // min
  18,   // hour
  27,    // day
  11,    // month
  2021, // year
  1,    // interval in days
};

// Constants for humidifier schedule
const schedule HUM_ON_SCHEDULE = {
  0,    // sec
  2,    // min
  22,   // hour
  27,    // day
  11,    // month
  2021, // year
  1,    // interval in days
};

const schedule HUM_OFF_SCHEDULE = {
  0,    // sec
  2,    // min
  18,   // hour
  27,    // day
  11,    // month
  2021, // year
  1,    // interval in days
};

/*
 ** Watering solution:
 * Amount of connected pumps: 1
 * Pump type: AD20P-1230C
 * Pump specs:
 *  - 12VDC
 *  - Hmax 300cm
 *  - Qmax 240L/H(66ml/s)
 *  - Qreal 180L/H(50ml/s) - highly depends on the environment like pipe length etc.
 *
 *
 *  est water amount: 250ml - time in seconds
 */
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