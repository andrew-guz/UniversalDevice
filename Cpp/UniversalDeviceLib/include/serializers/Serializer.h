#ifndef _SERIALIZER_H_
#define _SERIALIZER_H_

#include "ISerializer.h"
#include "MessageHeaderSerializer.h"
#include "MessageSerializer.h"
#include "ThermometerCurrentValueSerializer.h"

template<typename T>
class Serializer final
{
public:
    static nlohmann::json ToJson(const T& t)
    {
        auto serializer = CreateSerializer();
        return serializer->ToJson(t);
    }

    static nlohmann::json ToJson(const std::shared_ptr<T>& t)
    {
        auto serializer = CreateSerializer();
        return serializer->ToJson(t);
    }

    static T ToObject(const nlohmann::json& j)
    {
        auto serializer = CreateSerializer();
        return serializer->ToObject(j);
    }

    static std::shared_ptr<T> ToSharedObject(const nlohmann::json& j)
    {
        auto serializer = CreateSerializer();
        return serializer->ToObject(j);
    }

private:
    static std::shared_ptr<ISerializer<T>> CreateSerializer();
};

template<typename T>
std::shared_ptr<ISerializer<T>> Serializer<T>::CreateSerializer()
{
    return nullptr;
}

template<>
std::shared_ptr<ISerializer<MessageHeader>> Serializer<MessageHeader>::CreateSerializer()
{
    return std::make_shared<MessageHeaderSerializer>();
}

template<>
std::shared_ptr<ISerializer<Message>> Serializer<Message>::CreateSerializer()
{
    return std::make_shared<MessageSerializer>();
}

template<>
std::shared_ptr<ISerializer<ThermometerCurrentValue>> Serializer<ThermometerCurrentValue>::CreateSerializer()
{
    return std::make_shared<ThermometerCurrentValueSerializer>();
}

#endif //_SERIALIZER_H_
