#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

#include "Uuid.h"

class Parameters final
{
public:
    Parameters() = default;

    static Parameters ReadFromFile(const std::string fileName);

public:
    int     _port = 8080;
    Uuid    _id;
    double  _startTemperature = 20.0f;
    double  _delta = 0.1f;
};

#endif //_PARAMETERS_H_
