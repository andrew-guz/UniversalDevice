#pragma once

#include <chrono>

#include "DbExtension.hpp"
#include "IDb.hpp"
#include "Logger.hpp"
#include "RelayCurrentState.hpp"

struct ExtendedRelayCurrentState final : public RelayCurrentState, public IDb<ExtendedRelayCurrentState> {
    std::chrono::system_clock::time_point _timestamp;

    virtual std::vector<std::string> ToDbStrings() const override {
        LOG_ERROR << "This object should not be written to Database." << std::endl;
        return {};
    }

    virtual void FromDbStrings(const std::vector<std::string>& dbStrings) override {
        if (dbStrings.size() % 2 == 0) {
            auto timestamp = DbExtension::FindValueByName<std::chrono::system_clock::time_point>(dbStrings, "timestamp");
            auto state = DbExtension::FindValueByName<int>(dbStrings, "state");
            if (timestamp.has_value() && state.has_value()) {
                _timestamp = timestamp.value();
                _state = state.value();
            }
        } else
            LOG_ERROR << "Invalid db strings." << std::endl;
    }
};
