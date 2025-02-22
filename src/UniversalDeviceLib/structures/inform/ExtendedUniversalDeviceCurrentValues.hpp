#pragma once

#include <chrono>
#include <string>

#include <nlohmann/json.hpp>

#include "DbExtension.hpp"
#include "IDb.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "UniversalDeviceCurrentValues.hpp"

struct ExtendedUniversalDeviceCurrentValues final : public UniversalDeviceCurrentValues, public IDb<ExtendedUniversalDeviceCurrentValues> {
    std::chrono::system_clock::time_point _timestamp;

    virtual std::vector<std::string> ToDbStrings() const override {
        LOG_ERROR_MSG("This object should not be written to Database");
        return {};
    }

    virtual void FromDbStrings(const std::vector<std::string>& dbStrings) override {
        if (dbStrings.size() % 2 == 0) {
            auto timestamp = DbExtension::FindValueByName<std::chrono::system_clock::time_point>(dbStrings, "timestamp");
            auto values = DbExtension::FindValueByName<std::string>(dbStrings, "values");
            if (timestamp.has_value() && values.has_value()) {
                _timestamp = timestamp.value();
                _values = nlohmann::json::parse(values.value()).get<UniversalDeviceCurrentValues>()._values;
            }
        } else
            LOG_ERROR_MSG("Invalid db strings");
    }
};
