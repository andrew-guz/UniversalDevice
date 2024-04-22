#pragma once

#include "ComponentDescription.hpp"

struct DeviceInformationDescription : public ComponentDescription {
    uint64_t _seconds = 0;

    virtual nlohmann::json ToJson() const override {
        auto componentDescription = ComponentDescription::ToJson();
        componentDescription += {"seconds", _seconds};
        return componentDescription;
    }

    virtual void FromJson(const nlohmann::json& json) override {
        ComponentDescription::FromJson(json);
        _seconds = json.value("seconds", (uint64_t)0);
    }
};
