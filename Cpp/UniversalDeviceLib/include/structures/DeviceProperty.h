#ifndef _DEVICE_PROPERTY_H_
#define _DEVICE_PROPERTY_H_

#include "IJson.h"

struct DeviceProperty final : public IJson<DeviceProperty>
{
    std::string _value;

    virtual nlohmann::json ToJson() const override
    {
        return {
            { "value", _value }
        };  
    }

    virtual void FromJson(const nlohmann::json& json) override
    {
        _value = json.value("value", std::string());
    }
};

#endif //_DEVICE_PROPERTY_H_
