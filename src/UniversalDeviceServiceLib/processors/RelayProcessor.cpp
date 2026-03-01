#include "RelayProcessor.hpp"

#include <chrono>

#include <fmt/format.h>
#include <nlohmann/json_fwd.hpp>

#include "DeviceDataProcessor.hpp"
#include "Enums.hpp"
#include "Logger.hpp"
#include "Message.hpp"
#include "RelayValue.hpp"
#include "RelayValuesController.hpp"

RelayProcessor::RelayProcessor(RelayValuesController& relayValuesController) :
    DeviceDataProcessor<RelayValuesController, RelayValue, DeviceType::Relay>(relayValuesController) {}

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
    return Add(timestamp, message);
}

nlohmann::json RelayProcessor::ProcessGetDeviceInformationMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) {
    return Get(timestamp, message);
}
