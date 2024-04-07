#ifndef _MESSAGE_HELPER_H_
#define _MESSAGE_HELPER_H_

#include <vector>

#include "Logger.h"
#include "Message.h"

class MessageHelper final {
public:
    static Message Create(std::string_view type, const Uuid& id, std::string_view subject, const nlohmann::json& data);

    static Message Create(const ComponentDescription& description, std::string_view subject, const nlohmann::json& data);
};

#endif //_MESSAGE_HELPER_H_
