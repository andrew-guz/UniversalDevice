#include "MessageHeaderSerializer.h"

nlohmann::json MessageHeaderSerializer::ToJson(const MessageHeader& t) const
{
    return {
        {"from_type", t._from_type},
        {"from_id", t._from_id.data()},
        {"subject", t._subject}
    };  
}

nlohmann::json MessageHeaderSerializer::ToJson(const std::shared_ptr<MessageHeader>& t) const
{
    throw std::runtime_error("Simple object");
}

MessageHeader MessageHeaderSerializer::ToObject(const nlohmann::json& j) const
{
    MessageHeader header;
    header._from_type = j["from_type"].get<std::string>();
    header._from_id = Uuid(j["from_id"].get<std::string>());
    header._subject = j["subject"].get<std::string>();
    return header;
}

std::shared_ptr<MessageHeader> MessageHeaderSerializer::ToSharedObject(const nlohmann::json& j) const
{
    throw std::runtime_error("Simple object");
}
