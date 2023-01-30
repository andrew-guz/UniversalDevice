#ifndef _EXTENDED_COMPONENT_DESCRIPTION_H_
#define _EXTENDED_COMPONENT_DESCRIPTION_H_

#include <chrono>

#include "IDb.h"
#include "TimeHelper.h"
#include "ComponentDescription.h"
#include "Logger.h"

//since we derived from other Json<T> static method CreateFromJson will be ambiguous
struct ExtendedComponentDescription final : ComponentDescription, public IJson<ExtendedComponentDescription>, public IDb<ExtendedComponentDescription>
{
    std::string                             _name;
    std::chrono::system_clock::time_point   _timestamp;

    virtual nlohmann::json ToJson() const override
    {
        auto componentDescription = ComponentDescription::ToJson();
        componentDescription += { "name", _name };
        componentDescription += { "timestamp", TimeHelper::TimeToString(_timestamp) };
        return componentDescription;
    }

    virtual void FromJson(const nlohmann::json& json) override
    {
        ComponentDescription::FromJson(json);
        _name = json.value("name", "");
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
            LOG_ERROR << "Invalid db strings." << std::endl;
    }
};

#endif //_EXTENDED_COMPONENT_DESCRIPTION_H_
