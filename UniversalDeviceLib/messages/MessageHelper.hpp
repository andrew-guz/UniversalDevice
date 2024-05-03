#pragma once

#include "Logger.hpp"
#include "Marshaling.hpp"
#include "Message.hpp"

class MessageHelper final {
public:
    static Message Create(std::string_view type, const Uuid& id, std::string_view subject, const nlohmann::json& data);

    static Message Create(const ComponentDescription& description, std::string_view subject, const nlohmann::json& data);

    template<typename Object>
    static Message Create(std::string_view type, const Uuid& id, std::string_view subject, const Object& object) {
        return Create(type, id, subject, nlohmann::json(object));
    }

    template<typename Object>
    static Message Create(const ComponentDescription& description, std::string_view subject, const Object& object) {
        return Create(description, subject, nlohmann::json(object));
    }
};
