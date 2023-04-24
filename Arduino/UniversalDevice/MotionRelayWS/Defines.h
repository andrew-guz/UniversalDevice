#ifndef _DEFINES_H_
#define _DEFINES_H_

//uuid of current UniversalDevice
#define UUID ""

#define WIFI_ESP32
//#define WIFI_ESP8266

//wifi networks and passwords - specify before upload
#define SSID_MAIN "xxx"
#define SSID_ADDITIONAL "xxx"
#define PASSWORD_MAIN "xxx"
#define PASSWORD_ADDITIONAL "xxx"
#define AUTHORIZATION_STR "xxx"

//api
#define API_IP          "x.x.x.x"
#define API_PORT        7315
#define API_HTTPS_BASE  "https://" API_IP ":" API_PORT "/api/device"
#define API_INFORM      API_HTTPS_BASE "/inform"
#define API_SETTINGS    API_HTTPS_BASE "/settings"
#define API_COMMANDS    API_HTTPS_BASE "/commands"
#define API_WS          "/api/device/ws"

#endif //_DEFINES_H_
