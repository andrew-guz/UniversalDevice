#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

#include "Uuid.h"

class Parameters final
{
protected:
    Parameters() = default;

public:
    static Parameters ReadFromFile(const std::string fileName);

public:
    int     _port = 8080;
    Uuid    _id;
    double  _startTemperature = 20.0f;
    double  _delta = 0.1f;
    int     _sleepTimeout = 5;
};

#endif //_PARAMETERS_H_
