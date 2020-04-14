/**
 * 
 * Types.h 
 * 
 **/

#ifndef TYPES_H
#define TYPES_H

#include <Arduino.h>

enum EndpointType {
    POWER_ON,
    POWER_OFF,
    DEVICE_STATUS
};

typedef struct {
    String url;
    EndpointType type;
} endpoint;

#endif