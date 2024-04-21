#ifndef _MESSAGE_HELPER_H_
#define _MESSAGE_HELPER_H_

#include "Logger.h"
#include "Message.h"

class MessageHelper final {
public:
    static Message Create(std::string_view type, const Uuid& id, std::string_view subject, const nlohmann::json& data);

    static Message Create(const ComponentDescription& description, std::string_view subject, const nlohmann::json& data);

    static Message Create(std::string_view type, const Uuid& id, std::string_view subject);

    static Message Create(const ComponentDescription& description, std::string_view subject);

    template<typename Object>
    static Message Create(std::string_view type, const Uuid& id, std::string_view subject, const Object& object) {
        return Create(type, id, subject, object.ToJson());
    }

    template<typename Object>
    static Message Create(const ComponentDescription& description, std::string_view subject, const Object& object) {
        return Create(description, subject, object.ToJson());
    }
};

#endif //_MESSAGE_HELPER_H_
