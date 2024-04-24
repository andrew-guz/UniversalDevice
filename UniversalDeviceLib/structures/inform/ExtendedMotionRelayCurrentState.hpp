#pragma once

#include <chrono>

#include "DbExtension.hpp"
#include "IDb.hpp"
#include "Logger.hpp"
#include "MotionRelayCurrentState.hpp"
#include "TimeHelper.hpp"

struct ExtendedMotionRelayCurrentState final : public MotionRelayCurrentState,
                                               public IJson<ExtendedMotionRelayCurrentState>,
                                               public IDb<ExtendedMotionRelayCurrentState> {
    std::chrono::system_clock::time_point _timestamp;

    virtual nlohmann::json ToJson() const override {
        auto motionRelayCurrentState = MotionRelayCurrentState::ToJson();
        motionRelayCurrentState += {"timestamp", TimeHelper::TimeToInt(_timestamp)};
        return motionRelayCurrentState;
    }

    virtual void FromJson(const nlohmann::json& json) override {
        MotionRelayCurrentState::FromJson(json);
        _timestamp = TimeHelper::TimeFromInt(json.value("timestamp", (int64_t)0));
    }

    virtual std::vector<std::string> ToDbStrings() const override {
        LOG_ERROR << "This object should not be written to Database." << std::endl;
        return {};
    }

    virtual void FromDbStrings(const std::vector<std::string>& dbStrings) override {
        if (dbStrings.size() % 2 == 0) {
            auto timestamp = DbExtension::FindValueByName(dbStrings, "timestamp");
            auto motion = DbExtension::FindValueByName(dbStrings, "motion");
            auto state = DbExtension::FindValueByName(dbStrings, "state");
            if (timestamp.size() && motion.size() && state.size()) {
                _timestamp = TimeHelper::TimeFromInt((int64_t)std::stoll(timestamp));
                _motion = atoi(motion.c_str());
                _state = atoi(state.c_str());
            }
        } else
            LOG_ERROR << "Invalid db strings." << std::endl;
    }
};
