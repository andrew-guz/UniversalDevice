#ifndef _PERIOD_SETTINGS_H_
#define _PERIOD_SETTINGS_H_

#include <nlohmann/json.hpp>

#include "Defines.h"

struct PeriodSettings
{
    int _period = DEFAULT_PERIOD;

    nlohmann::json PeriodToJson() const
    {
        return {
            { "period", _period }
        };  
    }

    void PeriodFromJson(const nlohmann::json& json)
    {
        _period = json.value("period", DEFAULT_PERIOD);
    }
};

#endif //_PERIOD_SETTINGS_H_
