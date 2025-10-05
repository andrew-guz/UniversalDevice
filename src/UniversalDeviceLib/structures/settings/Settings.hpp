#pragma once

#include <variant>

#include "MotionRelaySettings.hpp"
#include "PeriodSettings.hpp"

using Settings = std::variant<PeriodSettings, MotionRelaySettings>;

inline bool operator==(const Settings& lhs, const Settings& rhs) {
    return lhs.index() == rhs.index() && std::visit([](auto&& l, auto&& r) -> bool { return l == r; }, lhs, rhs);
}
