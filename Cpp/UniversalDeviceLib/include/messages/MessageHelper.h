#ifndef _MESSAGE_HELPER_H_
#define _MESSAGE_HELPER_H_

#include <vector>

#include "Logger.h"
#include "Message.h"

class MessageHelper final
{
public:
    static Message Create(const std::string& fromType, const Uuid& fromId, const std::string& toType, const Uuid& toId, const std::string subject, const nlohmann::json& data);

    static Message Create(const ComponentDescription& from, const ComponentDescription& to, const std::string subject, const nlohmann::json& data);

    template<typename T>
    static std::vector<T> ParseMessage(const Message& message)
    {
        std::vector<T> result;
        if (!message.IsValid())
        {
            LOG_ERROR << "Invalid message." << std::endl;
            return result;
        }
        try
        {
            for (auto& json : message._data)
            {
                T obj;
                obj.FromJson(json);
                result.push_back(obj);
            }
        }
        catch(...)
        {
            LOG_ERROR << "Broken JSON in message data" << message._data.dump() << "." << std::endl;
            return result;
        }
        return result;
    }
};

#endif //_MESSAGE_HELPER_H_
