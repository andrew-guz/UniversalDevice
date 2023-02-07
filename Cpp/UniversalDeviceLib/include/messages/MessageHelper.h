#ifndef _MESSAGE_HELPER_H_
#define _MESSAGE_HELPER_H_

#include <vector>

#include "Logger.h"
#include "Message.h"

class MessageHelper final
{
public:
    static Message Create(const std::string& type, const Uuid& id, const std::string subject, const nlohmann::json& data);

    static Message Create(const ComponentDescription& description, const std::string subject, const nlohmann::json& data);
};

#endif //_MESSAGE_HELPER_H_
