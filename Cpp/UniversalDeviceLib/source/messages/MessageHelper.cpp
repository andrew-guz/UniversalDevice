#include "MessageHelper.h"

Message MessageHelper::Create(const std::string_view type, const Uuid& id, const std::string_view subject, const nlohmann::json& data) {
    ComponentDescription description;
    description._type = type;
    description._id = id;
    return MessageHelper::Create(description, subject, data);
}

Message MessageHelper::Create(const ComponentDescription& description, const std::string_view subject, const nlohmann::json& data) {
    Message message;
    message._header._description = description;
    message._header._subject = subject;
    message._data = data;
    return message;
}
