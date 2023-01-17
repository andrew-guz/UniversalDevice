#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

#include "Uuid.h"

class Parameters
{
public:
    Parameters() = delete;

    static Parameters ReadFromFile(const std::string fileName);

public:
    int     _port;
    Uuid    _id;
    double  _startTemperature;
    double  _delta;
    int     _sleepTimeout;
};

#endif //_PARAMETERS_H_
