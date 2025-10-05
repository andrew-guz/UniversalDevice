#pragma once

#include <chrono>

#include "UniversalDeviceCurrentValues.hpp"

struct ExtendedUniversalDeviceCurrentValues final : public UniversalDeviceCurrentValues {
    std::chrono::system_clock::time_point _timestamp;
};
