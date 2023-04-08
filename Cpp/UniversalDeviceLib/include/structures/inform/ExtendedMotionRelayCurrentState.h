#ifndef _EXTENDED_MOTION_RELAY_CURRENT_STATE_H_
#define _EXTENDED_MOTION_RELAY_CURRENT_STATE_H_

#include <chrono>

#include "MotionRelayCurrentState.h"
#include "IDb.h"
#include "DbExtension.h"
#include "Logger.h"
#include "TimeHelper.h"

struct ExtendedMotionRelayCurrentState final : public MotionRelayCurrentState, public IJson<ExtendedMotionRelayCurrentState>, public IDb<ExtendedMotionRelayCurrentState>
{
    std::chrono::system_clock::time_point _timestamp;

    virtual nlohmann::json ToJson() const override
    {
        auto motionRelayCurrentState = MotionRelayCurrentState::ToJson();
        motionRelayCurrentState += { "timestamp", TimeHelper::TimeToString(_timestamp) };
        return motionRelayCurrentState;
    }

    virtual void FromJson(const nlohmann::json& json) override
    {
        MotionRelayCurrentState::FromJson(json);
        _timestamp = TimeHelper::TimeFromString(json.value("timestamp", ""));
    }

    virtual std::vector<std::string> ToDbStrings() const override
    {
        LOG_ERROR << "This object should not be written to Database." << std::endl;
        return {};
    }

    virtual void FromDbStrings(const std::vector<std::string>& dbStrings) override
    {
        if (dbStrings.size() % 2 == 0)
        {
            auto timestamp = DbExtension::FindValueByName(dbStrings, "timestamp");
            auto motion = DbExtension::FindValueByName(dbStrings, "motion");
            auto state = DbExtension::FindValueByName(dbStrings, "state");
            if (timestamp.size() &&
                motion.size() &&
                state.size())
            {
                _timestamp = TimeHelper::TimeFromString(timestamp);
                _motion = atoi(motion.c_str());
                _state = atoi(state.c_str());
            }
        }
        else
            LOG_ERROR << "Invalid db strings." << std::endl;
    }
};

#endif //_EXTENDED_MOTION_RELAY_CURRENT_STATE_H_
