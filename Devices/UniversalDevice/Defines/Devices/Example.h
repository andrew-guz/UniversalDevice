#ifndef _DEVICE_H_
#define _DEVICE_H_

// uuid of current UniversalDevice
#define DEVICE_UUID ""

// #define BOARD_ESP32
// #define BOARD_ESP8266

// currently device can be either thermometer with/without led, relay or motion relay
// main features
// #define HAS_THERMOMETER
// #define HAS_RELAY
// #define HAS_MOTION_RELAY
// additional features
#ifdef HAS_THERMOMETER
// #define ONE_WIRE_TEMPERATURE_SENSOR
// #define I2C_TEMPERATURE_SENSOR
// #define HAS_DISPLAY
// #ifdef HAS_DISPLAY
// #define TM1637_DISPLAY
// #define OLED_DISPLAY
// #endif // HAS_DISPLAY
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
#ifdef HAS_DISPLAY
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

#endif //_DEVICE_H_
