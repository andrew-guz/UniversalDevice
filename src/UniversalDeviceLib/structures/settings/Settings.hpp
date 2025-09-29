#pragma once

#include <variant>

#include "MotionRelaySettings.hpp"
#include "PeriodSettings.hpp"

using Settings = std::variant<PeriodSettings, MotionRelaySettings>;
