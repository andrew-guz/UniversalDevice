#pragma once

#include <chrono>

#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include "DeviceInformationDescription.hpp"
#include "Enums.hpp"
#include "IProcessor.hpp"
#include "Marshaling.hpp"
#include "Message.hpp"
#include "Provider.hpp"

template<typename ControllerType, typename ValueType, DeviceType deviceType>
class DeviceDataProcessor : public IProcessor {
public:
    DeviceDataProcessor(ControllerType& controller) :
        _controller(controller) {}

    virtual ~DeviceDataProcessor() = default;

protected:
    nlohmann::json Add(const std::chrono::system_clock::time_point& timestamp, const Message& message) {
        ValueType value = message._data.get<ValueType>();
        value._timestamp = timestamp;

        if (!_controller.Add(GetProviderId(message._header._description), value))
            return {};

        return nlohmann::json{
            { "acknowledge", message._header._id },
        };
    }

    nlohmann::json Get(const std::chrono::system_clock::time_point& timestamp, const Message& message) {
        const DeviceInformationDescription description = message._data.get<DeviceInformationDescription>();
        if (description._id.isEmpty())
            return {};

        return _controller.Get(description._id, description._seconds);
    }

protected:
    ControllerType& _controller;
};
