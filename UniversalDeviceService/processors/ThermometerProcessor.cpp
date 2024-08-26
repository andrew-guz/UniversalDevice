#include "ThermometerProcessor.hpp"

#include <sstream>

#include <nlohmann/json_fwd.hpp>

#include "Constants.hpp"
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
    LOG_ERROR << "Unknown subject to process in ThermometerProcessor: " << static_cast<int>(message._header._subject) << std::endl;
    return {};
}

nlohmann::json ThermometerProcessor::ProcessThermometerCurrentValueMessage(const std::chrono::system_clock::time_point& timestamp,
                                                                           const Message& message) {
    auto currentValue = message._data.get<ThermometerCurrentValue>();
    if (currentValue._value == std::numeric_limits<float>::min()) {
        LOG_ERROR << "ThermometerProcessor - invalid message." << std::endl;
        return {};
    }
    auto& description = message._header._description;
    if (std::abs(currentValue._value - -127.0f) < 0.1f) {
        LOG_INFO << "-127.0 found - no sensor connected to " << description._id.data() << "." << std::endl;
        return {};
    }
    std::stringstream queryStream;
    queryStream << "INSERT INTO Thermometers (id, timestamp, value) VALUES ('" << description._id.data() << "', " << TimeHelper::TimeToInt(timestamp)
                << ", '" << currentValue._value << "')";
    queryStream.flush();
    if (!_queryExecutor->Execute(queryStream.str())) {
        LOG_SQL_ERROR(queryStream.str());
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
        std::stringstream queryStream;
        queryStream << "SELECT timestamp, value FROM Thermometers WHERE id = '" << description._id.data()
                    << "' AND timestamp >= " << TimeHelper::TimeToInt(now) << " AND value > -126.9 ORDER BY idx DESC";
        queryStream.flush();
        std::vector<std::vector<std::string>> data;
        if (_queryExecutor->Select(queryStream.str(), data))
            extendedThermometerCurrentValues = DbExtension::CreateVectorFromDbStrings<ExtendedThermometerCurrentValue>(data);
        else
            LOG_SQL_ERROR(queryStream.str());
    }
    if (extendedThermometerCurrentValues.size() == 0) {
        std::stringstream queryStream;
        queryStream << "SELECT timestamp, value FROM Thermometers WHERE id = '" << description._id.data()
                    << "' AND value > -126.9 ORDER BY idx DESC LIMIT 1";
        queryStream.flush();
        std::vector<std::vector<std::string>> data;
        if (_queryExecutor->Select(queryStream.str(), data))
            extendedThermometerCurrentValues = DbExtension::CreateVectorFromDbStrings<ExtendedThermometerCurrentValue>(data);
        else
            LOG_SQL_ERROR(queryStream.str());
    }
    if (extendedThermometerCurrentValues.size())
        return extendedThermometerCurrentValues;
    LOG_INFO << "No data for device " << description._id.data() << "found." << std::endl;
    return {};
}
