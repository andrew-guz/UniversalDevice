#pragma once

#include "Defines.hpp"
#include "PeriodSettings.hpp"

struct MotionRelaySettings final : public PeriodSettings {
    int _activityTime = DEFAULT_ACTIVITY_TIME;
};
