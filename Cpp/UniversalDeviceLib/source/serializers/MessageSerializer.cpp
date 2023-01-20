#include "MessageSerializer.h"

#include "Serializer.h"
#include "MessageHeaderSerializer.h"

nlohmann::json MessageSerializer::ToJson(const Message& t) const
{
    return {
        { "header", Serializer<MessageHeader>::ToJson(t._header) },
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
    message._header = Serializer<MessageHeader>::ToObject(j["header"]);
    message._data = j["data"];
    return message;
}

std::shared_ptr<Message> MessageSerializer::ToSharedObject(const nlohmann::json& j) const
{
    throw std::runtime_error("Simple object");
}