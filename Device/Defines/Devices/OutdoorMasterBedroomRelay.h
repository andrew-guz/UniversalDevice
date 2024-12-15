#ifndef _DEVICE_H_
#define _DEVICE_H_

#define DEVICE_UUID "26ad2731-5472-4d2d-95c2-daffca8d2992"

#define BOARD_ESP8266

#define HAS_RELAY
#define RELAY_AS_THERMOSTAT
#define RELAY_THERMOSTAT_VALUE 2.5f
#define RELAY_THERMOSTAT_DELTA 0.5f
#define RELAY_PIN              D5
#define RELAY_THERMOSTAT_PIN   D6

#endif //_DEVICE_H_