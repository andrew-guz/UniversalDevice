#ifndef _THERMOMETER_SETTINGS_H_
#define _THERMOMETER_SETTINGS_H_

#include <limits>

#include "IJson.h"

struct ThermometerSettings : public IJson<ThermometerSettings>
{
    int _period = 5000;

    virtual nlohmann::json ToJson() const override
    {
        return {
            { "period", _period }
        };  
    }

    virtual void FromJson(const nlohmann::json& json) override
    {
        _period = json.value("period", 5000);
    }
};

#endif //_THERMOMETER_SETTINGS_H_
