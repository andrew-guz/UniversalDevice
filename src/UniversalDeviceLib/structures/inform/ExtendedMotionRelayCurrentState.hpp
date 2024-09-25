#pragma once

#include <chrono>

#include "DbExtension.hpp"
#include "IDb.hpp"
#include "Logger.hpp"
#include "MotionRelayCurrentState.hpp"

struct ExtendedMotionRelayCurrentState final : public MotionRelayCurrentState, public IDb<ExtendedMotionRelayCurrentState> {
    std::chrono::system_clock::time_point _timestamp;

    virtual std::vector<std::string> ToDbStrings() const override {
        LOG_ERROR_MSG("This object should not be written to Database");
        return {};
    }

    virtual void FromDbStrings(const std::vector<std::string>& dbStrings) override {
        if (dbStrings.size() % 2 == 0) {
            auto timestamp = DbExtension::FindValueByName<std::chrono::system_clock::time_point>(dbStrings, "timestamp");
            auto motion = DbExtension::FindValueByName<bool>(dbStrings, "motion");
            auto state = DbExtension::FindValueByName<int>(dbStrings, "state");
            if (timestamp.has_value() && motion.has_value() && state.has_value()) {
                _timestamp = timestamp.value();
                _motion = motion.value();
                _state = state.value();
            }
        } else
            LOG_ERROR_MSG("Invalid db strings");
    }
};
