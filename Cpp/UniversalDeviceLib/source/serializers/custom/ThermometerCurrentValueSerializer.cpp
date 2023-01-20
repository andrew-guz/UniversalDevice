#include "ThermometerCurrentValueSerializer.h"

nlohmann::json ThermometerCurrentValueSerializer::ToJson(const ThermometerCurrentValue& t) const
{
    return {
        { "value", t._value }
    };  
}

nlohmann::json ThermometerCurrentValueSerializer::ToJson(const std::shared_ptr<ThermometerCurrentValue>& t) const
{
    throw std::runtime_error("Simple object");
}

ThermometerCurrentValue ThermometerCurrentValueSerializer::ToObject(const nlohmann::json& j) const
{
    ThermometerCurrentValue thermometerCurrentValue;
    thermometerCurrentValue._value = j["value"].get<float>();
    return thermometerCurrentValue;
}

std::shared_ptr<ThermometerCurrentValue> ThermometerCurrentValueSerializer::ToSharedObject(const nlohmann::json& j) const
{
    throw std::runtime_error("Simple object");
}
