/*
*
* Defines constants for endpoints
*
*/

#ifndef ENDPOINTS_H
#define ENDPOINTS_H

#include <Arduino.h>
#include "Types.h"


const endpoint LAMP_POWER_ON = {
    "/v2/relay/lamp/power/on",
    POWER_ON
};

const endpoint LAMP_POWER_OFF = {
    "/v2/relay/lamp/power/off",
    POWER_OFF
};

const endpoint LAMP_POWER_STATUS = {
    "/v2/relay/lamp/power/status",
    DEVICE_STATUS
};

const endpoint FAN_POWER_ON = {
    "/v2/relay/fan/power/on",
    POWER_ON
};

const endpoint FAN_POWER_OFF = {
    "/v2/relay/fan/power/off",
    POWER_OFF
};

const endpoint FAN_POWER_STATUS = {
    "/v2/relay/fan/power/status",
    DEVICE_STATUS
};

const endpoint HUM_POWER_ON = {
    "/v2/relay/hum/power/on",
    POWER_ON
};

const endpoint HUM_POWER_OFF = {
    "/v2/relay/hum/power/off",
    POWER_OFF
};

const endpoint HUM_POWER_STATUS = {
    "/v2/relay/hum/power/status",
    DEVICE_STATUS
};


#endif