#pragma once

#include "Defines.hpp"
#include "PeriodSettings.hpp"

struct MotionRelaySettings : public PeriodSettings, public IJson<MotionRelaySettings> {
    int _activityTime = DEFAULT_ACTIVITY_TIME;

    virtual nlohmann::json ToJson() const override {
        auto result = PeriodSettings::ToJson();
        result += {"activityTime", _activityTime};
        return result;
    }

    virtual void FromJson(const nlohmann::json& json) override {
        PeriodSettings::FromJson(json);
        _activityTime = json.value("activityTime", DEFAULT_ACTIVITY_TIME);
    }
};
