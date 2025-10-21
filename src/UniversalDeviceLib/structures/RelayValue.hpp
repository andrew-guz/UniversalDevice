#pragma once

#include <chrono>
#include <limits>
#include <optional>

struct RelayValue {
    int _state = std::numeric_limits<float>::min();

    std::optional<std::chrono::system_clock::time_point> _timestamp = std::nullopt;
};
