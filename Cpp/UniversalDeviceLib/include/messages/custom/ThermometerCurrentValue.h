#ifndef _THERMOMETER_CUSTOM_VALUE_H_
#define _THERMOMETER_CUSTOM_VALUE_H_

#include <limits>

#include "IJson.h"

struct ThermometerCurrentValue final : public IJson<ThermometerCurrentValue>
{
    float _value = std::numeric_limits<float>::min();

    virtual nlohmann::json ToJson() const override
    {
        return {
                { "value", _value }
            };  
    }

    virtual void FromJson(const nlohmann::json& json) override
    {
        _value = json.value("value", std::numeric_limits<float>::min());
    }
};

#endif //_THERMOMETER_CUSTOM_VALUE_H_
