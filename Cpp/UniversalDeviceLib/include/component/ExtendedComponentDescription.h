#ifndef _EXTENDED_COMPONENT_DESCRIPTION_H_
#define _EXTENDED_COMPONENT_DESCRIPTION_H_

#include <chrono>

#include "IDb.h"
#include "TimeHelper.h"
#include "ComponentDescription.h"
#include "Logger.h"
#include "DbExtension.h"

struct ExtendedComponentDescription final : ComponentDescription, public IJson<ExtendedComponentDescription>, public IDb<ExtendedComponentDescription>
{
    std::string                             _name;
    std::string                             _group;
    std::chrono::system_clock::time_point   _timestamp;

    std::string GetDisplayString() const
    {
        return _name.size() ? _name : _id.data();
    }

    virtual nlohmann::json ToJson() const override
    {
        auto componentDescription = ComponentDescription::ToJson();
        componentDescription += { "name", _name };
        componentDescription += { "grp", _group };
        componentDescription += { "timestamp", TimeHelper::TimeToInt(_timestamp) };
        return componentDescription;
    }

    virtual void FromJson(const nlohmann::json& json) override
    {
        ComponentDescription::FromJson(json);
        _name = json.value("name", "");
        _group = json.value("grp", "");
        _timestamp = TimeHelper::TimeFromInt(json.value("timestamp", (int64_t)0));
    }

    virtual std::vector<std::string> ToDbStrings() const override
    {
        //id, type, name, group, timestamp
        return {
            _id.data(),
            _type,
            _name,
            _group,
            std::to_string(TimeHelper::TimeToInt(_timestamp))
        };
    }

    virtual void FromDbStrings(const std::vector<std::string>& dbStrings) override
    {
        if (dbStrings.size() % 2 == 0)
        {
            auto type = DbExtension::FindValueByName(dbStrings, "type");
            auto id = DbExtension::FindValueByName(dbStrings, "id");
            auto name = DbExtension::FindValueByName(dbStrings, "name");
            auto group = DbExtension::FindValueByName(dbStrings, "grp");
            auto timestamp = DbExtension::FindValueByName(dbStrings, "timestamp");
            if (type.size() &&
                id.size() &&
                timestamp.size())
            {
                _type = type;
                _id = id;
                _name = name;
                _group = group;
                _timestamp = TimeHelper::TimeFromInt((int64_t)std::stoll(timestamp));
            }
        }
        else
            LOG_ERROR << "Invalid db strings." << std::endl;
    }
};

#endif //_EXTENDED_COMPONENT_DESCRIPTION_H_
