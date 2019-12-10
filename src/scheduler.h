/*
 * scheduler.h
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <Arduino.h>
#include <ezTime.h>

class Scheduler {
public:
  void (*function)();
  uint8_t startSec, startMin, startHr, startDay, startMnth;
  uint16_t startYear;
  uint8_t intervalDays;

  Scheduler(
    void (*function)(),
    uint8_t startSec,
    uint8_t startMin,
    uint8_t startHr,
    uint8_t startDay,
    uint8_t startMnth,
    uint16_t startYear,
    uint8_t intervalDays,
    Timezone *tz
    );

  Scheduler(
    void (*function)(),
    uint8_t startHr,
    uint8_t startDay,
    uint8_t startMnth,
    uint16_t startYear,
    uint8_t intervalDays,
    Timezone *tz
    ) :
      Scheduler(function, 0, 0, startHr, startDay, startMnth, startYear, intervalDays, tz) {};

  time_t getStartUnixTime();
  time_t getNextUnixTime();

  /*
   * Print Date/Time in a customizable format
   * Available formats are listed here: https://github.com/ropg/ezTime#datetime
   */
  String getStartDateTime(const String format = DEFAULT_TIMEFORMAT);
  String getNextDateTime(const String format = DEFAULT_TIMEFORMAT);

private:
  Timezone *_tz;
};

#endif
