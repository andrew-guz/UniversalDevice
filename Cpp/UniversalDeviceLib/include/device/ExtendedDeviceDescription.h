#ifndef _EXTENDED_DEVICE_DESCRIPTION_H_
#define _EXTENDED_DEVICE_DESCRIPTION_H_

#include <iostream>
#include <chrono>

#include "IDb.h"
#include "TimeHelper.h"
#include "DeviceDescription.h"

struct ExtendedDeviceDescription final : DeviceDescription, public IJson<ExtendedDeviceDescription>, public IDb<ExtendedDeviceDescription>
{
    std::string                             _name;
    std::chrono::system_clock::time_point   _timestamp;

    virtual nlohmann::json ToJson() const override
    {
        auto deviceDescription = DeviceDescription::ToJson();
        deviceDescription += { "name", _name };
        deviceDescription += { "timestamp", TimeHelper::TimeToString(_timestamp) };
        return deviceDescription;
    }

    virtual void FromJson(const nlohmann::json& json) override
    {
        DeviceDescription::FromJson(json);
        _id = json.value("name", "");
        _timestamp = TimeHelper::TimeFromString(json.value("timestamp", ""));
    }

    virtual std::vector<std::string> ToDbStrings() const override
    {
        //id, type, name, timestamp
        return {
            _id.data(),
            _type,            
            _name,
            TimeHelper::TimeToString(_timestamp)
        };
    }

    virtual void FromDbStrings(const std::vector<std::string>& dbStrings) override
    {
        if(dbStrings.size() == 4)
        {
            //id, type, name, timestamp
            _type = dbStrings[1];
            _id = dbStrings[0];
            _name = dbStrings[2];
            _timestamp = TimeHelper::TimeFromString(dbStrings[3]);
        }
        else
            std::cout << "Invalid db strings" << std::endl;
    }
};

#endif //_EXTENDED_DEVICE_DESCRIPTION_H_
