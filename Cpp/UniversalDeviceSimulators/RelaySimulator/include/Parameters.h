#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

#include "Defines.h"
#include "Uuid.h"

class Parameters final {
public:
    Parameters() = default;

    static Parameters ReadFromFile(const std::string fileName);

public:
    int _port = DEFAULT_SERVICE_PORT;
    Uuid _id;
};

#endif //_PARAMETERS_H_
