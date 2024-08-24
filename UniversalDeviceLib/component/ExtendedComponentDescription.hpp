#pragma once

#include <chrono>

#include "ComponentDescription.hpp"
#include "DbExtension.hpp"
#include "IDb.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "TimeHelper.hpp"
#include "Uuid.hpp"

struct ExtendedComponentDescription final : ComponentDescription, public IDb<ExtendedComponentDescription> {
    std::string _name;
    std::string _group;
    std::chrono::system_clock::time_point _timestamp;

    std::string GetDisplayString() const { return _name.size() ? _name : _id.data(); }

    std::string GetDisplayStringWithGroup() const {
        return _group.empty() ? GetDisplayString() : GetDisplayString() + std::string{ " (" } + _group + std::string{ ")" };
    }

    virtual std::vector<std::string> ToDbStrings() const override {
        // id, type, name, group, timestamp
        return {
            _id.data(), ActorTypeToString(_type), _name, _group, std::to_string(TimeHelper::TimeToInt(_timestamp)),
        };
    }

    virtual void FromDbStrings(const std::vector<std::string>& dbStrings) override {
        if (dbStrings.size() % 2 == 0) {
            auto type = DbExtension::FindValueByName<ActorType>(dbStrings, "type");
            auto id = DbExtension::FindValueByName<Uuid>(dbStrings, "id");
            auto name = DbExtension::FindValueByName<std::string>(dbStrings, "name");
            auto group = DbExtension::FindValueByName<std::string>(dbStrings, "grp");
            auto timestamp = DbExtension::FindValueByName<std::chrono::system_clock::time_point>(dbStrings, "timestamp");
            if (type.has_value() && id.has_value() && timestamp.has_value()) {
                _type = type.value();
                _id = id.value();
                _name = name.value_or(std::string{});
                _group = group.value_or(std::string{});
                _timestamp = timestamp.value();
            }
        } else
            LOG_ERROR << "Invalid db strings." << std::endl;
    }
};
