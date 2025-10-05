#pragma once

#include <chrono>

#include "RelayCurrentState.hpp"

struct ExtendedRelayCurrentState final : public RelayCurrentState {
    std::chrono::system_clock::time_point _timestamp;
};
