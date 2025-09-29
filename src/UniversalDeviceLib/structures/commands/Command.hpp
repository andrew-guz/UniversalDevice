#pragma once

#include <variant>

#include "RelayState.hpp"
#include "ThermometerLedBrightness.hpp"

using Command = std::variant<RelayState, ThermometerLedBrightness>;
