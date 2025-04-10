#include "UniversalDeviceProcessor.hpp"

#include <fmt/format.h>
#include <nlohmann/json.hpp>

#include "DeviceInformationDescription.hpp"
#include "ExtendedUniversalDeviceCurrentValues.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "TimeHelper.hpp"
#include "UniversalDeviceCurrentValues.hpp"

UniversalDeviceProcessor::UniversalDeviceProcessor(IQueryExecutor* queryExecutor) :
    BaseProcessorWithQueryExecutor(queryExecutor) {}

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
    auto currentValues = message._data.get<UniversalDeviceCurrentValues>();
    if (currentValues._values.empty()) {
        LOG_ERROR_MSG("UniversalDeviceProcessor - invalid message - no values");
        return {};
    }
    auto& description = message._header._description;
    const std::string query = fmt::format("INSERT INTO UniversalDevices (id, timestamp, 'values') VALUES ('{}', {}, '{}')",
                                          description._id.data(),
                                          TimeHelper::TimeToInt(timestamp),
                                          message._data.dump());
    if (!_queryExecutor->Execute(query)) {
        LOG_SQL_ERROR(query);
        return {};
    }
    return nlohmann::json{
        { "acknowledge", message._header._id },
    };
}

nlohmann::json UniversalDeviceProcessor::ProcessGetDeviceInformationMessage(const std::chrono::system_clock::time_point& timestamp,
                                                                            const Message& message) {
    auto description = message._data.get<DeviceInformationDescription>();
    if (!description.isDeviceType() || description.getDeviceType() != DeviceType::UniversalDevice || description._id.isEmpty())
        return {};
    std::vector<ExtendedUniversalDeviceCurrentValues> extendedUniversalDeviceCurrentValues;
    if (description._seconds != 0) {
        auto now = std::chrono::system_clock::now();
        now -= std::chrono::seconds(description._seconds);
        const std::string query =
            fmt::format("SELECT timestamp, \"values\" FROM UniversalDevices WHERE id = '{}' AND timestamp >= {} ORDER BY idx DESC",
                        description._id.data(),
                        TimeHelper::TimeToInt(now));
        std::vector<std::vector<std::string>> data;
        if (_queryExecutor->Select(query, data))
            extendedUniversalDeviceCurrentValues = DbExtension::CreateVectorFromDbStrings<ExtendedUniversalDeviceCurrentValues>(data);
        else
            LOG_SQL_ERROR(query);
    }
    if (extendedUniversalDeviceCurrentValues.size() == 0) {
        const std::string query =
            fmt::format("SELECT timestamp, \"values\" FROM UniversalDevices WHERE id = '{}' ORDER BY idx DESC LIMIT 1", description._id.data());
        std::vector<std::vector<std::string>> data;
        if (_queryExecutor->Select(query, data))
            extendedUniversalDeviceCurrentValues = DbExtension::CreateVectorFromDbStrings<ExtendedUniversalDeviceCurrentValues>(data);
        else
            LOG_SQL_ERROR(query);
    }
    if (extendedUniversalDeviceCurrentValues.size())
        return extendedUniversalDeviceCurrentValues;
    LOG_INFO_MSG(fmt::format("No data for device {} found", description._id.data()));
    return {};
}
