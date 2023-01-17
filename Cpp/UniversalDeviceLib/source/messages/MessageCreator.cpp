#include "MessageCreator.h"

Message MessageCreator::Create(const std::string &type, const Uuid &id, const std::string subject, const nlohmann::json& data)
{
    Message message;
    message._header._from_type = type;
    message._header._from_id = id;
    message._header._subject = subject;
    message._data = data;
    return message;
}
