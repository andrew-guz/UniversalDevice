#include "ThermometerProcessor.hpp"

#include <optional>

#include <fmt/format.h>

#include "DeviceInformationDescription.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "ThermometerValue.hpp"
#include "ThermometerValuesController.hpp"

ThermometerProcessor::ThermometerProcessor(IQueryExecutor* queryExecutor, ThermometerValuesController& thermometerValuesController) :
    BaseProcessorWithQueryExecutor(queryExecutor),
    _thermometerValuesController(thermometerValuesController) {}

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
    ThermometerValue value = message._data.get<ThermometerValue>();
    value._timestamp = timestamp;

    if (!_thermometerValuesController.Add(message._header._description._id, value))
        return {};

    return nlohmann::json{
        { "acknowledge", message._header._id },
    };
}

nlohmann::json ThermometerProcessor::ProcessGetDeviceInformationMessage(const std::chrono::system_clock::time_point& timestamp,
                                                                        const Message& message) {
    auto description = message._data.get<DeviceInformationDescription>();
    if (!description.isDeviceType() || description.getDeviceType() != DeviceType::Thermometer || description._id.isEmpty())
        return {};

    return _thermometerValuesController.Get(description._id, description._seconds);
}
