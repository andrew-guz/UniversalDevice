#include "RelayProcessor.hpp"

#include <optional>

#include <fmt/format.h>

#include "DeviceInformationDescription.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "RelayValue.hpp"
#include "RelayValuesController.hpp"

RelayProcessor::RelayProcessor(IQueryExecutor* queryExecutor, RelayValuesController& relayValuesController) :
    BaseProcessorWithQueryExecutor(queryExecutor),
    _relayValuesController(relayValuesController) {}

nlohmann::json RelayProcessor::ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) {
    switch (message._header._subject) {
        case Subject::RelayCurrentState:
            return ProcessRelayCurrentStateMessage(timestamp, message);
        case Subject::GetDeviceInformation:
            return ProcessGetDeviceInformationMessage(timestamp, message);
        case Subject::Undefined:
        case Subject::TimerEvent:
        case Subject::ThermometerCurrentValue:
        case Subject::MotionRelayCurrentState:
        case Subject::WebSocketAuthorization:
        case Subject::WebSocketGetSettings:
        case Subject::WebSocketGetCommands:
        case Subject::UniversalDeviceCurrentState:
        case Subject::SunriseEvent:
        case Subject::SunsetEvent:
            return {};
    }
    LOG_ERROR_MSG(fmt::format("Unknown subject to process in RelayProcessor: {}", static_cast<int>(message._header._subject)));
    return {};
}

nlohmann::json RelayProcessor::ProcessRelayCurrentStateMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) {
    RelayValue value = message._data.get<RelayValue>();
    value._timestamp = timestamp;

    if (!_relayValuesController.Add(message._header._description._id, value))
        return {};

    return nlohmann::json{
        { "acknowledge", message._header._id },
    };
}

nlohmann::json RelayProcessor::ProcessGetDeviceInformationMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) {
    auto description = message._data.get<DeviceInformationDescription>();
    if (!description.isDeviceType() || description.getDeviceType() != DeviceType::Relay || description._id.isEmpty())
        return {};

    return _relayValuesController.Get(description._id, description._seconds);
}
