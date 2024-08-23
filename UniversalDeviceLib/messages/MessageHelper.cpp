#include "MessageHelper.hpp"

Message MessageHelper::Create(const ComponentDescription& description, const std::string_view subject, const nlohmann::json& data) {
    Message message;
    message._header._description = description;
    message._header._subject = subject;
    message._data = data;
    return message;
}
