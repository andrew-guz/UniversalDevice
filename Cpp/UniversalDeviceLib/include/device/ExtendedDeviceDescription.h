#ifndef _EXTENDED_DEVICE_DESCRIPTION_H_
#define _EXTENDED_DEVICE_DESCRIPTION_H_

#include "DeviceDescription.h"

struct ExtendedDeviceDescription final : DeviceDescription, public IJson<ExtendedDeviceDescription>
{
    std::string _name;

    virtual nlohmann::json ToJson() const override
    {
        auto deviceDescription = DeviceDescription::ToJson();
        deviceDescription += { "name", _name };
        return deviceDescription;
    }

    virtual void FromJson(const nlohmann::json& json) override
    {
        DeviceDescription::FromJson(json);
        _id = json.value("name", "");
    }
};

#endif //_EXTENDED_DEVICE_DESCRIPTION_H_
