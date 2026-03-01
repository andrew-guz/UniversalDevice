#pragma once

#include <variant>

#include "RelayState.hpp"
#include "ThermometerLedBrightness.hpp"

using Command = std::variant<RelayState, ThermometerLedBrightness>;

inline bool operator==(const RelayState& lhs, const RelayState& rhs) { return lhs._state == rhs._state; }

inline bool operator==(const ThermometerLedBrightness& lhs, const ThermometerLedBrightness& rhs) { return lhs._brightness == rhs._brightness; }

inline bool operator==(const Command& lhs, const Command& rhs) {
    return lhs.index() == rhs.index() && std::visit([](const auto& l, const auto& r) -> bool { return l == r; }, lhs, rhs);
}
