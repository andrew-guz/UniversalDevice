#pragma once

#include <chrono>
#include <limits>
#include <optional>

struct MotionRelayValue {
    bool _motion = false;

    int _state = std::numeric_limits<int>::min();

    std::optional<std::chrono::system_clock::time_point> _timestamp = std::nullopt;
};
