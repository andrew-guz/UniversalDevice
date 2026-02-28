#include "UniversalDeviceProcessor.hpp"

#include <chrono>

#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include "DeviceDataProcessor.hpp"
#include "Enums.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "UniversalValue.hpp"
#include "UniversalValuesController.hpp"

UniversalDeviceProcessor::UniversalDeviceProcessor(UniversalValuesController& universalValuesController) :
    DeviceDataProcessor<UniversalValuesController, UniversalValue, DeviceType::UniversalDevice>(universalValuesController) {}

nlohmann::json UniversalDeviceProcessor::ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) {
    switch (message._header._subject) {
        case Subject::UniversalDeviceCurrentState:
            return ProcessUniversalDeviceCurrentStateMessage(timestamp, message);
        case Subject::GetDeviceInformation:
            return ProcessGetDeviceInformationMessage(timestamp, message);
        case Subject::Undefined:
        case Subject::TimerEvent:
        case Subject::ThermometerCurrentValue:
        case Subject::RelayCurrentState:
        case Subject::MotionRelayCurrentState:
        case Subject::WebSocketAuthorization:
        case Subject::WebSocketGetSettings:
        case Subject::WebSocketGetCommands:
        case Subject::SunriseEvent:
        case Subject::SunsetEvent:
            return {};
    }
    LOG_ERROR_MSG(fmt::format("Unknown subject to process in UniversalDeviceProcessor: {}", static_cast<int>(message._header._subject)));
    return {};
}

nlohmann::json UniversalDeviceProcessor::ProcessUniversalDeviceCurrentStateMessage(const std::chrono::system_clock::time_point& timestamp,
                                                                                   const Message& message) {
    return Add(timestamp, message);
}

nlohmann::json UniversalDeviceProcessor::ProcessGetDeviceInformationMessage(const std::chrono::system_clock::time_point& timestamp,
                                                                            const Message& message) {
    return Get(timestamp, message);
}
