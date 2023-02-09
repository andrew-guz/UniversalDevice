#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include <string>

class Constants final
{
public:
    //login
    static const std::string LoginService;
    static const std::string LoginDevice;

    // device types   
    static const std::string DeviceTypeUndefined;
    static const std::string DeviceTypeThermometer;

    //subjects - depends on type
    static const std::string SubjectUndefined;
    static const std::string SubjectGetDeviceInformation;
    static const std::string SubjectThermometerCurrentValue;
};

#endif //_CONSTANTS_H_
