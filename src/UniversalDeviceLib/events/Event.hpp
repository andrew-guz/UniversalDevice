#pragma once

#include <variant>

#include "RelayEvent.hpp"
#include "SunriseEvent.hpp"
#include "SunsetEvent.hpp"
#include "ThermometerEvent.hpp"
#include "ThermostatEvent.hpp"
#include "TimerEvent.hpp"

using Event = std::variant<TimerEvent, ThermometerEvent, RelayEvent, ThermostatEvent, SunriseEvent, SunsetEvent>;
