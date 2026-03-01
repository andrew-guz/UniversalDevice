#include "ThermometerProcessor.hpp"

#include <chrono>

#include <fmt/format.h>
#include <nlohmann/json_fwd.hpp>

#include "DeviceDataProcessor.hpp"
#include "Enums.hpp"
#include "Logger.hpp"
#include "Message.hpp"
#include "ThermometerValue.hpp"
#include "ThermometerValuesController.hpp"

ThermometerProcessor::ThermometerProcessor(ThermometerValuesController& thermometerValuesController) :
    DeviceDataProcessor<ThermometerValuesController, ThermometerValue, DeviceType::Thermometer>(thermometerValuesController) {}

nlohmann::json ThermometerProcessor::ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) {
    switch (message._header._subject) {
        case Subject::ThermometerCurrentValue:
            return ProcessThermometerCurrentValueMessage(timestamp, message);
        case Subject::GetDeviceInformation:
            return ProcessGetDeviceInformationMessage(timestamp, message);
        case Subject::Undefined:
        case Subject::TimerEvent:
        case Subject::RelayCurrentState:
        case Subject::MotionRelayCurrentState:
        case Subject::WebSocketAuthorization:
        case Subject::WebSocketGetSettings:
        case Subject::WebSocketGetCommands:
        case Subject::UniversalDeviceCurrentState:
        case Subject::SunriseEvent:
        case Subject::SunsetEvent:
            return {};
    }
    LOG_ERROR_MSG(fmt::format("Unknown subject to process in ThermometerProcessor: {}", static_cast<int>(message._header._subject)));
    return {};
}

nlohmann::json ThermometerProcessor::ProcessThermometerCurrentValueMessage(const std::chrono::system_clock::time_point& timestamp,
                                                                           const Message& message) {
    return Add(timestamp, message);
}

nlohmann::json ThermometerProcessor::ProcessGetDeviceInformationMessage(const std::chrono::system_clock::time_point& timestamp,
                                                                        const Message& message) {
    return Get(timestamp, message);
}
