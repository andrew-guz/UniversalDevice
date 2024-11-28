#pragma once

#include <limits>

struct ThermometerCurrentValue {
    float _value = std::numeric_limits<float>::min();

    static constexpr float InvalidTemperature = -127.0f;
};
