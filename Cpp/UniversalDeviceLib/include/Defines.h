#ifndef _DEFINES_H_
#define _DEFINES_H_

#define VERSION "1.0"

#define DEFAULT_SERVICE_PORT 7315
#define DEFAULT_FRONTEND_PORT 7316

#define DEFAULT_PERIOD 5000

#define MIN_BRIGHTNESS 0
#define MAX_BRIGHTNESS 7

#define DEFAULT_ACTIVITY_TIME 60000

//api addresses
//common
#define API_VERSION                         "/api/version"
#define API_QUIT                            "/api/quit"
//for devices
#define API_DEVICE_SETTINGS                 "/api/device/settings/<string>"
#define API_DEVICE_COMMANDS                 "/api/device/commands/<string>"
#define API_DEVICE_INFORM                   "/api/device/inform"
//for client
#define API_CLIENT_DEVICES                  "/api/client/list"
#define API_CLIENT_DEVICE_NAME              "/api/client/device/name/<string>"
#define API_CLIENT_DEVICE_GET_INFO          "/api/client/device"
#define API_CLIENT_EVENTS                   "/api/client/events"

#endif //_DEFINES_H_
