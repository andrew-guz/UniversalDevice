#pragma once

#include <chrono>

struct CurrentTime final {
    std::chrono::system_clock::time_point _timestamp;
};
