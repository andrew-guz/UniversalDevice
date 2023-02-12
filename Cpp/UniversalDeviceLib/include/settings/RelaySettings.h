#ifndef _RELAY_SETTINGS_H_
#define _RELAY_SETTINGS_H_

#include "IJson.h"
#include "PeriodSettings.h"

struct RelaySettings final : public PeriodSettings, public IJson<RelaySettings>
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

#endif //_RELAY_SETTINGS_H_
