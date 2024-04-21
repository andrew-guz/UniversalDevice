#pragma once

#include "Defines.h"
#include "Uuid.h"

class BaseParameters {
public:
    BaseParameters() = default;

    static BaseParameters ReadFromFile();

public:
    Uuid _id;
};
