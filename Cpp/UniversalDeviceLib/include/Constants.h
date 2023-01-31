#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include <string>

class Constants final
{
public:
    //service types
    static const std::string DeviceServiceType;
    static const std::string ClientServiceType;
    static const std::string FrontendServiceType;

    // device types   
    static const std::string DeviceTypeUndefined;
    static const std::string DeviceTypeThermometer;

    //subjects - depends on type
    static const std::string SubjectUndefined;
    static const std::string SubjectThermometerCurrentValue;
    static const std::string SubjectGetDeviceInformation;
};

#endif //_CONSTANTS_H_
