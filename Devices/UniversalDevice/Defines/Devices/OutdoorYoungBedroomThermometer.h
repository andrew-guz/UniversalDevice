#ifndef _DEVICE_H_
#define _DEVICE_H_

#define DEVICE_UUID "c8ed77e6-e1ba-4e9d-9049-a97669d7ea14"
#define DEVICE_NAME "Young_Thermometer"

#define BOARD_ESP8266

#define HAS_THERMOMETER
#define ONE_WIRE_TEMPERATURE_SENSOR
#define HAS_DISPLAY
#define TM1637_DISPLAY
#define THERMOMETER_PIN D5
#define LED_CLK_PIN     D6
#define LED_DIO_PIN     D7

#endif //_DEVICE_H_
