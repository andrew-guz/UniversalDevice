#include "MotionRelayProcessor.hpp"

#include <optional>

#include <fmt/format.h>

#include "DeviceInformationDescription.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "MotionRelayValue.hpp"
#include "MotionRelayValuesController.hpp"

MotionRelayProcessor::MotionRelayProcessor(IQueryExecutor* queryExecutor, MotionRelayValuesController& motionRelayValuesController) :
    BaseProcessorWithQueryExecutor(queryExecutor),
    _motionRelayValuesController(motionRelayValuesController) {}

nlohmann::json MotionRelayProcessor::ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) {
    switch (message._header._subject) {
        case Subject::MotionRelayCurrentState:
            return ProcessMotionRelayCurrentStateMessage(timestamp, message);
        case Subject::GetDeviceInformation:
            return ProcessGetDeviceInformationMessage(timestamp, message);
        case Subject::Undefined:
        case Subject::TimerEvent:
        case Subject::ThermometerCurrentValue:
        case Subject::RelayCurrentState:
        case Subject::WebSocketAuthorization:
        case Subject::WebSocketGetSettings:
        case Subject::WebSocketGetCommands:
        case Subject::UniversalDeviceCurrentState:
        case Subject::SunriseEvent:
        case Subject::SunsetEvent:
            return {};
    }
    LOG_ERROR_MSG(fmt::format("Unknown subject to process in MotionRelayProcessor: {}", static_cast<int>(message._header._subject)));
    return {};
}

nlohmann::json MotionRelayProcessor::ProcessMotionRelayCurrentStateMessage(const std::chrono::system_clock::time_point& timestamp,
                                                                           const Message& message) {
    MotionRelayValue value = message._data.get<MotionRelayValue>();
    value._timestamp = timestamp;

    if (!_motionRelayValuesController.Add(message._header._description._id, value))
        return {};

    return nlohmann::json{
        { "acknowledge", message._header._id },
    };
}

nlohmann::json MotionRelayProcessor::ProcessGetDeviceInformationMessage(const std::chrono::system_clock::time_point& timestamp,
                                                                        const Message& message) {
    auto description = message._data.get<DeviceInformationDescription>();
    if (!description.isDeviceType() || description.getDeviceType() != DeviceType::MotionRelay || description._id.isEmpty())
        return {};

    return _motionRelayValuesController.Get(description._id, description._seconds);
}
