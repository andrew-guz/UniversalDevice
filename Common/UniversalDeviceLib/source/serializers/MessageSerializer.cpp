#include "MessageSerializer.h"

#include "MessageHeaderSerializer.h"

nlohmann::json MessageSerializer::ToJson(const Message& t) const
{
    MessageHeaderSerializer header_serializer;
    return {
        { "header", header_serializer.ToJson(t._header) },
        { "data", t._data }
    };
}

nlohmann::json MessageSerializer::ToJson(const std::shared_ptr<Message>& t) const
{
    throw std::runtime_error("Simple object");
}

Message MessageSerializer::ToObject(const nlohmann::json& j) const
{
    MessageHeaderSerializer header_serializer;
    Message message;
    message._header = header_serializer.ToObject(j["header"]);
    message._data = j["data"];
    return message;
}

std::shared_ptr<Message> MessageSerializer::ToSharedObject(const nlohmann::json& j) const
{
    throw std::runtime_error("Simple object");
}