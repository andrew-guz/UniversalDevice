#include "MessageCreator.h"

Message MessageCreator::Create(const std::string& fromType, const Uuid& fromId, const std::string& toType, const Uuid& toId, const std::string subject, const nlohmann::json& data)
{
    ComponentDescription from;
    from._type = fromType;
    from._id = fromId;
    ComponentDescription to;
    to._type = toType;
    to._id = toId;
    return MessageCreator::Create(from, to, subject, data);
}

Message MessageCreator::Create(const ComponentDescription& from, const ComponentDescription& to, const std::string subject, const nlohmann::json& data)
{
    Message message;
    message._header._from = from;
    message._header._to = to;
    message._header._subject = subject;
    message._data = data;
    return message;
}
