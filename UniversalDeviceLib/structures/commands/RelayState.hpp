#pragma once

#include <limits>

struct RelayState final {
    int _state = std::numeric_limits<float>::min();
};
