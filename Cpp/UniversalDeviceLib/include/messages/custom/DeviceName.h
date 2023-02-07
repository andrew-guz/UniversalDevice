#ifndef _DEVICE_NAME_H_
#define _DEVICE_NAME_H_

#include "IJson.h"

struct DeviceName final : public IJson<DeviceName>
{
    std::string _name;

    virtual nlohmann::json ToJson() const override
    {
        return {
            { "name", _name }
        };  
    }

    virtual void FromJson(const nlohmann::json& json) override
    {
        _name = json.value("name", std::string());
    }
};

#endif //_DEVICE_NAME_H_
