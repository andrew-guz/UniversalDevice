#pragma once

#include <chrono>

#include "DbExtension.hpp"
#include "IDb.hpp"
#include "Logger.hpp"
#include "ThermometerCurrentValue.hpp"
#include "TimeHelper.hpp"

struct ExtendedThermometerCurrentValue final : public ThermometerCurrentValue, public IDb<ExtendedThermometerCurrentValue> {
    std::chrono::system_clock::time_point _timestamp;

    virtual std::vector<std::string> ToDbStrings() const override {
        LOG_ERROR << "This object should not be written to Database." << std::endl;
        return {};
    }

    virtual void FromDbStrings(const std::vector<std::string>& dbStrings) override {
        if (dbStrings.size() % 2 == 0) {
            auto timestamp = DbExtension::FindValueByName(dbStrings, "timestamp");
            auto value = DbExtension::FindValueByName(dbStrings, "value");
            if (timestamp.size() && value.size()) {
                _timestamp = TimeHelper::TimeFromInt((int64_t)std::stoll(timestamp));
                _value = atof(value.c_str());
            }
        } else
            LOG_ERROR << "Invalid db strings." << std::endl;
    }
};
