#ifndef _PERIOD_SETTINGS_H_
#define _PERIOD_SETTINGS_H_

#include <nlohmann/json.hpp>

#include "Defines.h"
#include "IJson.h"

struct PeriodSettings : public IJson<PeriodSettings> {
    int _period = DEFAULT_PERIOD;

    virtual nlohmann::json ToJson() const override { return {{"period", _period}}; }

    virtual void FromJson(const nlohmann::json& json) override { _period = json.value("period", DEFAULT_PERIOD); }
};

#endif //_PERIOD_SETTINGS_H_
