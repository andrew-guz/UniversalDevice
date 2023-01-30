#include "MessageCreator.h"

Message MessageCreator::Create(const ComponentDescription& from, const ComponentDescription& to, const std::string subject, const nlohmann::json& data)
{
    Message message;
    message._header._from = from;
    message._header._to = to;
    message._header._subject = subject;
    message._data = data;
    return message;
}
