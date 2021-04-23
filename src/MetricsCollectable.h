/**
 * MetricsCollectable.h
 **/

/**
 * Device which supposed to be scraped for Prometheus metrics must
 * inherit this abstract class (interface) and implement its methods
 **/

#ifndef METRICSCOLLECTABLE_H
#define METRICSCOLLECTABLE_H

#include <Arduino.h>

class MetricsCollectable {

  public:
    virtual void collect(String *metrics) = 0;
};

#endif
