#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include "Uuid.h"

class Constants final
{
public:
    //login
    static const std::string LoginService;
    static const std::string LoginDevice;

    // device types   
    static const std::string DeviceTypeUndefined;
    static const std::string DeviceTypeTimer;
    static const std::string DeviceTypeThermometer;
    static const std::string DeviceTypeRelay;
    static const std::string DeviceTypeMotionRelay;

    //subjects - depends on type
    static const std::string SubjectUndefined;
    static const std::string SubjectTimerEvent;
    static const std::string SubjectGetDeviceInformationSingle;
    static const std::string SubjectGetDeviceInformationMultiple;
    static const std::string SubjectThermometerCurrentValue;
    static const std::string SubjectRelayCurrentState;
    static const std::string SubjectMotionRelayCurrentState;

    //predefined ids for devices
    static const Uuid PredefinedIdTimer;

    //event types
    static const std::string EventTypeUndefined;
    static const std::string EventTypeTimer;
    static const std::string EventTypeThermometer;
    static const std::string EventTypeRelay;
    static const std::string EventTypeThermostat;
};

#endif //_CONSTANTS_H_
