/*
 * scheduler.cpp
 */

#include "scheduler.h"

Scheduler::Scheduler(
    void (*function)(),
    uint8_t startSec,
    uint8_t startMin,
    uint8_t startHr,
    uint8_t startDay,
    uint8_t startMnth,
    uint16_t startYear,
    uint8_t intervalDays,
    Timezone *tz
    ) :
      function(function),
      startSec(startSec),
      startMin(startMin),
      startHr(startHr),
      startDay(startDay),
      startMnth(startMnth),
      startYear(startYear),
      intervalDays(intervalDays),
      _tz(tz)
    {};

time_t Scheduler::getStartUnixTime() {
  return makeTime(startHr, startMin, startSec, startDay, startMnth, startYear);
}

time_t Scheduler::getNextUnixTime() {

  time_t t = getStartUnixTime();
  time_t intervalDaySecs = intervalDays * SECS_PER_DAY;

  while (t <= _tz->now())
  {
    t += intervalDaySecs;
  }

  return t;
};

String Scheduler::getStartDateTime(const String format) {
  return _tz->dateTime(getStartUnixTime(), format);
}

String Scheduler::getNextDateTime(const String format) {
  return _tz->dateTime(getNextUnixTime(), format);
}
