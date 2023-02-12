#ifndef _THERMOMETER_SETTINGS_H_
#define _THERMOMETER_SETTINGS_H_

#include "IJson.h"
#include "PeriodSettings.h"

struct ThermometerSettings final : public PeriodSettings, public IJson<ThermometerSettings>
{
    virtual nlohmann::json ToJson() const override
    {
        return PeriodToJson();
    }

    virtual void FromJson(const nlohmann::json& json) override
    {
        PeriodFromJson(json);
    }
};

#endif //_THERMOMETER_SETTINGS_H_
