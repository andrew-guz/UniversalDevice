#pragma once

#include <chrono>

#include "ThermometerCurrentValue.hpp"

struct ExtendedThermometerCurrentValue final : public ThermometerCurrentValue {
    std::chrono::system_clock::time_point _timestamp;
};
