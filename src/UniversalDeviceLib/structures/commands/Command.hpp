#pragma once

#include <variant>

#include "RelayState.hpp"
#include "ThermometerLedBrightness.hpp"

using Command = std::variant<RelayState, ThermometerLedBrightness>;

inline bool operator==(const Command& lhs, const Command& rhs) {
    return lhs.index() == rhs.index() && std::visit([](auto&& l, auto&& r) -> bool { return l == r; }, lhs, rhs);
}
