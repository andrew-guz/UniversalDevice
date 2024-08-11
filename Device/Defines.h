#ifndef _DEFINES_H_
#define _DEFINES_H_

// uuid of current UniversalDevice
#define DEVICE_UUID ""

// #define WIFI_ESP32
// #define WIFI_ESP8266

// currently device can be either thermometer with/without led, relay or motion relay
// main features
// #define HAS_THERMOMETER
// #define HAS_RELAY
// #define HAS_MOTION_RELAY
// additional features
#ifdef HAS_THERMOMETER
// #define HAS_LED
#endif
#if defined HAS_RELAY || defined HAS_MOTION_RELAY
// #define RELAY_SWITCH_ON_BY_LOW_LEVEL
#endif
#ifdef HAS_RELAY
// #define RELAY_AS_THERMOSTAT
// #define RELAY_THERMOSTAT_VALUE 2.5f
// #define RELAY_THERMOSTAT_DELTA 0.5f
#endif
// pins
#ifdef HAS_THERMOMETER
// #define THERMOMETER_PIN
#endif
#ifdef HAS_LED
// #define LED_CLK_PIN
// #define LED_DIO_PIN
#endif
#ifdef HAS_RELAY
// #define RELAY_PIN
#endif
#ifdef HAS_MOTION_RELAY
// #define MOTION_RELAY_DETECTOR_PIN
// #define MOTION_RELAY_RELAY_PIN
#endif
#ifdef RELAY_AS_THERMOSTAT
// #define RELAY_THERMOSTAT_PIN
#endif

// wifi networks and passwords - specify before upload
#define SSID_MAIN "xxx"
#define SSID_ADDITIONAL "xxx"
#define PASSWORD_MAIN "xxx"
#define PASSWORD_ADDITIONAL "xxx"
#define AUTHORIZATION_STR "xxx"

// api
#define API_IP "x.x.x.x"
#define API_PORT 7315
#define API_WS "/api/device/ws"

#endif //_DEFINES_H_
