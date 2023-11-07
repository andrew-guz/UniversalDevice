#ifndef _DEFINES_H_
#define _DEFINES_H_

//uuid of current UniversalDevice
#define UUID ""

//#define WIFI_ESP32
//#define WIFI_ESP8266

//currently device can be either  termoter with/without led or relay
//#define HAS_THERMOMETER
//#define THERMOMETER_PIN
//#define HAS_LED
//#define LED_CLK_PIN
//#define LED_DIO_PIN
//#define HAS_RELAY
//#define RELAY_PIN
//#define RELAY_SWITCH_ON_BY_LOW_LEVEL

//wifi networks and passwords - specify before upload
#define SSID_MAIN           "xxx"
#define SSID_ADDITIONAL     "xxx"
#define PASSWORD_MAIN       "xxx"
#define PASSWORD_ADDITIONAL "xxx"
#define AUTHORIZATION_STR   "xxx"

//api
#define API_IP          "x.x.x.x"
#define API_PORT        7315
#define API_WS          "/api/device/ws"

#endif //_DEFINES_H_
