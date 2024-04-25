#pragma once

#include <limits>

#include "IJson.hpp"

struct ThermometerCurrentValue : public IJson<ThermometerCurrentValue> {
    float _value = std::numeric_limits<float>::min();

    virtual nlohmann::json ToJson() const override { return { { "value", _value } }; }

    virtual void FromJson(const nlohmann::json& json) override { _value = json.value("value", std::numeric_limits<float>::min()); }
};
