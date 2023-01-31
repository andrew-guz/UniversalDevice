#ifndef _EXTENDED_THERMOMETER_CUSTOM_VALUE_H_
#define _EXTENDED_THERMOMETER_CUSTOM_VALUE_H_

#include <chrono>

#include "ThermometerCurrentValue.h"
#include "IDb.h"
#include "Logger.h"
#include "TimeHelper.h"

struct ExtendedThermometerCurrentValue final : public ThermometerCurrentValue, public IJson<ExtendedThermometerCurrentValue>, public IDb<ExtendedThermometerCurrentValue>
{
    std::chrono::system_clock::time_point _timestamp;

    virtual nlohmann::json ToJson() const override
    {
        auto thermometerCurrentValue = ThermometerCurrentValue::ToJson();
        thermometerCurrentValue += { "timestamp", TimeHelper::TimeToString(_timestamp) };
        return thermometerCurrentValue;
    }

    virtual void FromJson(const nlohmann::json& json) override
    {
        ThermometerCurrentValue::FromJson(json);
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
            auto value = DbExtension::FindValueByName(dbStrings, "value");
            if (timestamp.size() &&
                value.size())
            {
                _timestamp = TimeHelper::TimeFromString(timestamp);
                _value = atof(value.c_str());
            }
        }
        else
            LOG_ERROR << "Invalid db strings." << std::endl;
    }
};

#endif //_EXTENDED_THERMOMETER_CUSTOM_VALUE_H_
