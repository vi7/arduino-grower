/*
 * scheduler.h
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#define DEFAULT_LOCATION "Europe/Amsterdam"

#include <Arduino.h>
#include <ezTime.h>

typedef struct {
  uint8_t sec;
  uint8_t min;
  uint8_t hr;
  uint8_t day;
  uint8_t mnth;
  uint16_t year;
  uint8_t intervalDays;
} schedule;

class Scheduler {
public:
  void (*function)();
  uint8_t startSec, startMin, startHr, startDay, startMnth;
  uint16_t startYear;
  uint8_t intervalDays;

  Scheduler(){};

// TODO: replace start* fields with startUnixTime field initialized in constructor. 
  Scheduler(void (*function)(), schedule schedule, String* location = new String(DEFAULT_LOCATION)):
     function(function),
     startSec(schedule.sec),
     startMin(schedule.min),
     startHr(schedule.hr),
     startDay(schedule.day),
     startMnth(schedule.mnth),
     startYear(schedule.year),
     intervalDays(schedule.intervalDays) {
         this->_tz = new Timezone();
         initTimezone(location);
         setNextEvent();
     };

  time_t getStartUnixTime();
  time_t getNextUnixTime();

  /*
   * Print Date/Time in a customizable format
   * Available formats are listed here: https://github.com/ropg/ezTime#datetime
   */
  String getStartDateTime(const String format = DEFAULT_TIMEFORMAT);
  String getNextDateTime(const String format = DEFAULT_TIMEFORMAT);

  void initTimezone(String*);
  void setNextEvent();

private:
  Timezone *_tz;
};

#endif
