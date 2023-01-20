#ifndef _THERMOMETER_CURRENT_VALUE_SERIALIZER_H_
#define _THERMOMETER_CURRENT_VALUE_SERIALIZER_H_

#include "ISerializer.h"
#include "ThermometerCurrentValue.h"

class ThermometerCurrentValueSerializer final : public ISerializer<ThermometerCurrentValue>
{
public:
    ThermometerCurrentValueSerializer() = default;

    virtual ~ThermometerCurrentValueSerializer() = default;

    virtual nlohmann::json ToJson(const ThermometerCurrentValue& t) const override;

    virtual nlohmann::json ToJson(const std::shared_ptr<ThermometerCurrentValue>& t) const override;

    virtual ThermometerCurrentValue ToObject(const nlohmann::json& j) const override;

    virtual std::shared_ptr<ThermometerCurrentValue> ToSharedObject(const nlohmann::json& j) const override;
};

#endif //_THERMOMETER_CURRENT_VALUE_SERIALIZER_H_
