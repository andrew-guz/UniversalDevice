#pragma once

#include <limits>

#include "IJson.hpp"

struct RelayState : public IJson<RelayState> {
    int _state = std::numeric_limits<float>::min();

    virtual nlohmann::json ToJson() const override { return {{"state", _state}}; }

    virtual void FromJson(const nlohmann::json& json) override { _state = json.value("state", std::numeric_limits<float>::min()); }
};
