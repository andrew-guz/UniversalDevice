#ifndef _DEFINES_H_
#define _DEFINES_H_

//uuid of current UniversalDevice
#define UUID "XXX"

//#define WIFI_ESP32
//#define WIFI_ESP8266

//#define USE_LED

//wifi networks and passwords - specify before upload
#define SSID_MAIN "XXX"
#define SSID_ADDITIONAL "XXX"
#define PASSWORD_MAIN "XXX"
#define PASSWORD_ADDITIONAL "XXX"
#define AUTHORIZATION_STR "Basic XXX"

//api
#define API_BASE        "https://XXX:XXX/api/device"
#define API_INFORM      API_BASE "/inform"
#define API_SETTINGS    API_BASE "/settings"
#define API_COMMANDS    API_BASE "/commands"

#endif //_DEFINES_H_
