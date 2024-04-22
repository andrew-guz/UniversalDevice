#pragma once

#include "Uuid.h"

class Parameters final {
public:
    Parameters() = default;

    static Parameters ReadFromFile();

public:
    double _startTemperature = 20.0f;
    double _delta = 0.1f;
};
