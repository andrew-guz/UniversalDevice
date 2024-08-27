#include "ThermometerProcessor.hpp"

#include <fmt/format.h>

#include "DeviceInformationDescription.hpp"
#include "ExtendedThermometerCurrentValue.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "ThermometerCurrentValue.hpp"
#include "TimeHelper.hpp"

ThermometerProcessor::ThermometerProcessor(IQueryExecutor* queryExecutor) : BaseProcessorWithQueryExecutor(queryExecutor) {}

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
            return {};
    }
    LOG_ERROR_MSG(fmt::format("Unknown subject to process in ThermometerProcessor: {}", static_cast<int>(message._header._subject)));
    return {};
}

nlohmann::json ThermometerProcessor::ProcessThermometerCurrentValueMessage(const std::chrono::system_clock::time_point& timestamp,
                                                                           const Message& message) {
    auto currentValue = message._data.get<ThermometerCurrentValue>();
    if (currentValue._value == std::numeric_limits<float>::min()) {
        LOG_ERROR_MSG("ThermometerProcessor - invalid message");
        return {};
    }
    auto& description = message._header._description;
    if (std::abs(currentValue._value - -127.0f) < 0.1f) {
        LOG_INFO_MSG(fmt::format("-127.0 found - no sensor connected to {}", description._id.data()));
        return {};
    }
    const std::string query = fmt::format("INSERT INTO Thermometers (id, timestamp, value) VALUES ('{}', {}, '{}')", description._id.data(),
                                          TimeHelper::TimeToInt(timestamp), currentValue._value);
    if (!_queryExecutor->Execute(query)) {
        LOG_SQL_ERROR(query);
        return {};
    }
    return nlohmann::json{
        { "acknowledge", message._header._id },
    };
}

nlohmann::json ThermometerProcessor::ProcessGetDeviceInformationMessage(const std::chrono::system_clock::time_point& timestamp,
                                                                        const Message& message) {
    auto description = message._data.get<DeviceInformationDescription>();
    if (!description.isDeviceType() || description.getDeviceType() != DeviceType::Thermometer || description._id.isEmpty())
        return {};
    std::vector<ExtendedThermometerCurrentValue> extendedThermometerCurrentValues;
    if (description._seconds != 0) {
        auto now = std::chrono::system_clock::now();
        now -= std::chrono::seconds(description._seconds);
        const std::string query =
            fmt::format("SELECT timestamp, value FROM Thermometers WHERE id = '{}' AND timestamp >= {} AND value > -126.9 ORDER BY idx DESC",
                        description._id.data(), TimeHelper::TimeToInt(now));
        std::vector<std::vector<std::string>> data;
        if (_queryExecutor->Select(query, data))
            extendedThermometerCurrentValues = DbExtension::CreateVectorFromDbStrings<ExtendedThermometerCurrentValue>(data);
        else
            LOG_SQL_ERROR(query);
    }
    if (extendedThermometerCurrentValues.size() == 0) {
        const std::string query = fmt::format(
            "SELECT timestamp, value FROM Thermometers WHERE id = '{}' AND value > -126.9 ORDER BY idx DESC LIMIT 1", description._id.data());
        std::vector<std::vector<std::string>> data;
        if (_queryExecutor->Select(query, data))
            extendedThermometerCurrentValues = DbExtension::CreateVectorFromDbStrings<ExtendedThermometerCurrentValue>(data);
        else
            LOG_SQL_ERROR(query);
    }
    if (extendedThermometerCurrentValues.size())
        return extendedThermometerCurrentValues;
    LOG_INFO_MSG(fmt::format("No data for device {} found", description._id.data()));
    return {};
}
