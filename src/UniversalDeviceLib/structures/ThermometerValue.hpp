#pragma once

#include <chrono>
#include <limits>
#include <optional>

struct ThermometerValue {
    float _value = std::numeric_limits<float>::min();

    std::optional<std::chrono::system_clock::time_point> _timestamp = std::nullopt;

    static constexpr float InvalidTemperature = -127.0f;
};
