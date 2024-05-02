#pragma once

#include <limits>

struct MotionRelayCurrentState {
    bool _motion = false;
    int _state = std::numeric_limits<float>::min();
};
