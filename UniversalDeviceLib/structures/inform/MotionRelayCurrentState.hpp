#pragma once

#include <limits>

#include "IJson.hpp"

struct MotionRelayCurrentState : public IJson<MotionRelayCurrentState> {
    bool _motion = false;
    int _state = std::numeric_limits<float>::min();

    virtual nlohmann::json ToJson() const override { return { { "motion", _motion }, { "state", _state } }; }

    virtual void FromJson(const nlohmann::json& json) override {
        _motion = json.value("motion", false);
        _state = json.value("state", std::numeric_limits<float>::min());
    }
};
