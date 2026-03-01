#include "MotionRelayProcessor.hpp"

#include <chrono>

#include <fmt/format.h>
#include <nlohmann/json_fwd.hpp>

#include "DeviceDataProcessor.hpp"
#include "Enums.hpp"
#include "Logger.hpp"
#include "Message.hpp"
#include "MotionRelayValue.hpp"
#include "MotionRelayValuesController.hpp"

MotionRelayProcessor::MotionRelayProcessor(MotionRelayValuesController& motionRelayValuesController) :
    DeviceDataProcessor<MotionRelayValuesController, MotionRelayValue, DeviceType::MotionRelay>(motionRelayValuesController) {}

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
    return Add(timestamp, message);
}

nlohmann::json MotionRelayProcessor::ProcessGetDeviceInformationMessage(const std::chrono::system_clock::time_point& timestamp,
                                                                        const Message& message) {
    return Get(timestamp, message);
}
