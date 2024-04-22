#pragma once

#include "Defines.h"
#include "Uuid.h"

class BaseParameters {
public:
    BaseParameters() = default;

    static BaseParameters ReadFromFile();

public:
    int _port = DEFAULT_SERVICE_PORT;
    Uuid _id;
};
