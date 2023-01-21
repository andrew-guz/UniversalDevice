#include "MessageCreator.h"

Message MessageCreator::Create(const DeviceDescription& deviceDescription, const std::string subject, const nlohmann::json& data)
{
    Message message;
    message._header._deviceDescription = deviceDescription;
    message._header._subject = subject;
    message._data = data;
    return message;
}
