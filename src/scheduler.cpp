/*
 * scheduler.cpp
 */

#include "scheduler.h"

Scheduler::Scheduler() {
};

void Scheduler::init(void (*function)(), schedule schedule, String *location) {
  this->function = function;

  startSec = schedule.sec;
  startMin = schedule.min;
  startHr = schedule.hr;
  startDay = schedule.day;
  startMnth = schedule.mnth;
  startYear = schedule.year;
  intervalDays = schedule.intervalDays;

  this->_tz = new Timezone();
  initTimezone(location);
  setNextEvent();
}

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

void Scheduler::initTimezone(String *location) {
  // ezTime logging level, one of: NONE, ERROR, INFO, DEBUG
  ezt::setDebug(NONE);
  ezt::waitForSync();

  if (_tz->setCache(0)) {
    // TODO add check if location has been changed
    Serial.println(F("Using cached timezone info"));
  } else {
    Serial.println(F("Timezone info not found in cache, fetching it"));
    _tz->setLocation(*location);
  }
  // TODO extract method for current time logging and remove from scheduler init
	Serial.println(*location + " time: " + _tz->dateTime());
}

void Scheduler::setNextEvent() {
    _tz->setEvent(function, getNextUnixTime());
}