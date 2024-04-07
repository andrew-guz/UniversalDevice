#ifndef _MOTION_RELAY_SETTINGS_H_
#define _MOTION_RELAY_SETTINGS_H_

#include "Defines.h"
#include "PeriodSettings.h"

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

#endif //_MOTION_RELAY_SETTINGS_H_
