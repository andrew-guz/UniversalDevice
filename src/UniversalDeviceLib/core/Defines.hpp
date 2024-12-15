#pragma once

#define DEFAULT_SERVICE_PORT       7315
#define DEFAULT_FRONTEND_PORT      7316

#define DEFAULT_PERIOD             5000

#define MIN_BRIGHTNESS             0
#define MAX_BRIGHTNESS             7

#define MIN_ACTIVITY_DELAY         1
#define MAX_ACTIVITY_DELAY         5

#define DEFAULT_ACTIVITY_TIME      60000

#define LOG_RECORDS_COUT           300

// api addresses
// common
#define API_VERSION                "/api/version"
#define API_QUIT                   "/api/quit"
// for devices
#define API_DEVICE                 "/api/device/<string>"
#define API_DEVICE_SETTINGS        "/api/device/settings/<string>"
#define API_DEVICE_COMMANDS        "/api/device/commands/<string>"
// for devices websockets
#define API_DEVICE_WEBSOCKETS      "/api/device/ws"
// for client
#define API_CLIENT_DEVICES         "/api/client/list"
#define API_CLIENT_DEVICE_NAME     "/api/client/device/name/<string>"
#define API_CLIENT_DEVICE_GROUP    "/api/client/device/group/<string>"
#define API_CLIENT_DEVICE_GET_INFO "/api/client/device"
#define API_CLIENT_EVENTS          "/api/client/events"
#define API_CLIENT_SCENARIOS       "/api/client/scenarios"
#define API_CLIENT_SCENARIOS_ID    "/api/client/scenarios/<string>"
#define API_CLIENT_LOGS            "/api/client/backendLog"
#define API_CLIENT_RESTART_DEVICE  "/api/client/restart/<string>"
#define API_CLIENT_FIRMWARE        "/api/client/firmware/<string>"
