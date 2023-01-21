#ifndef _DEVICE_DESCRIPTION_H_
#define _DEVICE_DESCRIPTION_H_

#include <string>

#include "IJson.h"
#include "Constants.h"
#include "Uuid.h"

struct DeviceDescription : public IJson<DeviceDescription>
{
    std::string _type = Constants::DeviceTypeUndefined;
    Uuid        _id;

    virtual nlohmann::json ToJson() const override
    {
        return {
            { "type", _type },
            { "id", _id.data() }
        };
    }

    virtual void FromJson(const nlohmann::json& json) override
    {
        _type = json.value("type", Constants::DeviceTypeUndefined);
        _id = Uuid(json.value("id", ""));
    }
};

#endif //_DEVICE_DESCRIPTION_H_
