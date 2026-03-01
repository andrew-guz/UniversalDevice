#pragma once

#include <nlohmann/json_fwd.hpp>

#include "Enums.hpp"
#include "Marshaling.hpp"
#include "Message.hpp"
#include "Provider.hpp"

class MessageHelper final {
public:
    static Message CreateDeviceMessage(DeviceType deviceType, const Uuid& deviceId, Subject subject, const nlohmann::json& data);

    static Message CreateClientMessage(Subject subject, const nlohmann::json& data);

    static Message CreateEventMessage(const Uuid& eventId, Subject subject, const nlohmann::json& data);

    template<typename Object>
    static Message CreateDeviceMessage(const DeviceType deviceType, const Uuid& deviceId, Subject subject, const Object& object) {
        return CreateDeviceMessage(deviceType, deviceId, subject, nlohmann::json(object));
    }

    template<typename Object>
    static Message CreateClientMessage(const Subject subject, const Object& object) {
        return CreateClientMessage(subject, nlohmann::json(object));
    }

    template<typename Object>
    static Message CreateEventMessage(const Uuid& eventId, const Subject subject, const Object& object) {
        return CreateEventMessage(eventId, subject, nlohmann::json(object));
    }
};
