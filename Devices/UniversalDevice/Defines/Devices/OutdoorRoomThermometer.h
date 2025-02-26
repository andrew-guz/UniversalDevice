#ifndef _DEVICE_H_
#define _DEVICE_H_

#define DEVICE_UUID "24aded7a-f90a-446c-a04b-24fcb630ff41"
#define DEVICE_NAME "Room_Thermometer"

#define BOARD_ESP8266

#define HAS_THERMOMETER
#define ONE_WIRE_TEMPERATURE_SENSOR
#define HAS_DISPLAY
#define TM1637_DISPLAY
#define THERMOMETER_PIN D5
#define LED_CLK_PIN     D6
#define LED_DIO_PIN     D7

#endif //_DEVICE_H_
