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


endpoint LAMP_ENDPOINTS[] = {LAMP_POWER_ON, LAMP_POWER_OFF, LAMP_POWER_STATUS};

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

endpoint FAN_ENDPOINTS[] = {FAN_POWER_ON, FAN_POWER_OFF, FAN_POWER_STATUS};

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

endpoint HUM_ENDPOINTS[] = {HUM_POWER_ON, HUM_POWER_OFF, HUM_POWER_STATUS};

const endpoint WATER_STATUS = {
    "/v2/water/schedule/start",
    DEVICE_STATUS
};

endpoint WATER_ENDPOINTS[] = {WATER_STATUS};

const endpoint TEMP_STATUS = {
    "/v2/dht/temperature",
    DEVICE_STATUS
};

const endpoint HUM_STATUS = {
    "/v2/dht/humidity",
    DEVICE_STATUS
};

endpoint HT_ENDPOINTS[] = {TEMP_STATUS, HUM_STATUS};


const endpoint LDR_STATUS = {
    "/v2/ldr/brightness",
    DEVICE_STATUS
};
endpoint LDR_ENDPOINTS[] = {LDR_STATUS};

#endif