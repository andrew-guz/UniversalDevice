#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include <string>

class Constants final
{
public:
    // device types
    static const std::string DeviceTypeUndefined;
    static const std::string DeviceTypeThermometer;

    //subjects - depends on type
    static const std::string SubjectUndefined;
    static const std::string SubjectThermometerCurrentValue;
};

#endif //_CONSTANTS_H_
