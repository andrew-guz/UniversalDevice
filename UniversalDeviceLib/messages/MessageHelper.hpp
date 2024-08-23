#pragma once

#include "Marshaling.hpp"
#include "Message.hpp"

class MessageHelper final {
public:
    static Message Create(const ComponentDescription& description, std::string_view subject, const nlohmann::json& data);

    template<typename Type>
    static Message Create(Type type, const Uuid& id, std::string_view subject, const nlohmann::json& data) {
        ComponentDescription description;
        description._type = type;
        description._id = id;
        return MessageHelper::Create(description, subject, data);
    }

    template<typename Object>
    static Message Create(const ComponentDescription& description, std::string_view subject, const Object& object) {
        return Create(description, subject, nlohmann::json(object));
    }

    template<typename Type, typename Object>
    static Message Create(Type type, const Uuid& id, std::string_view subject, const Object& object) {
        ComponentDescription description;
        description._type = type;
        description._id = id;
        return MessageHelper::Create<Object>(description, subject, object);
    }
};
