/*
 * scheduler.cpp
 */

#include "scheduler.h"

time_t Scheduler::getNextUnixTime() {

  time_t t = startUnixTime;
  time_t intervalDaySecs = intervalDays * SECS_PER_DAY;

  while (t <= _tz->now()) {
    t += intervalDaySecs;
  }

  return t;
};

String Scheduler::getStartDateTime(const String format) {
  return _tz->dateTime(startUnixTime, format);
}

String Scheduler::getNextDateTime(const String format) {
  return _tz->dateTime(getNextUnixTime(), format);
}

void Scheduler::initTimezone(String *location) {
  // ezTime logging level, one of: NONE, ERROR, INFO, DEBUG
  ezt::setDebug(NONE);
  ezt::waitForSync();

  if (!_tz->setCache(0) && _tz->getOlson() != *location) {
    Serial.println(F("Timezone info not found in cache, fetching it"));
    _tz->setLocation(*location);
  }
  
  // TODO extract method for current time logging and remove from scheduler init
	Serial.println(*location + " time: " + _tz->dateTime());
}

void Scheduler::setNextEvent() {
    _tz->setEvent(function, getNextUnixTime());
}