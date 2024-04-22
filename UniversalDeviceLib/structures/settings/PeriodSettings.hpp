#pragma once

#include <nlohmann/json.hpp>

#include "Defines.hpp"
#include "IJson.hpp"

struct PeriodSettings : public IJson<PeriodSettings> {
    int _period = DEFAULT_PERIOD;

    virtual nlohmann::json ToJson() const override { return {{"period", _period}}; }

    virtual void FromJson(const nlohmann::json& json) override { _period = json.value("period", DEFAULT_PERIOD); }
};
