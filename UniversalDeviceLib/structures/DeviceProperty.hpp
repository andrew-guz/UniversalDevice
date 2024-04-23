#pragma once

#include "IJson.hpp"

struct DeviceProperty final : public IJson<DeviceProperty> {
    std::string _value;

    virtual nlohmann::json ToJson() const override { return {{"value", _value}}; }

    virtual void FromJson(const nlohmann::json& json) override { _value = json.value("value", std::string()); }
};
