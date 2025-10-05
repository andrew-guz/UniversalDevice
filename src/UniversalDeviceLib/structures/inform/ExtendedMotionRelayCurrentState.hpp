#pragma once

#include <chrono>

#include "MotionRelayCurrentState.hpp"

struct ExtendedMotionRelayCurrentState final : public MotionRelayCurrentState {
    std::chrono::system_clock::time_point _timestamp;
};
