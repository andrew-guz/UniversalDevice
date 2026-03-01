#pragma once

#include <chrono>
#include <map>
#include <optional>
#include <string>

#include "UniversalData.hpp"

struct UniversalValue {
    std::map<std::string, UniversalData> _values;

    std::optional<std::chrono::system_clock::time_point> _timestamp = std::nullopt;
};
