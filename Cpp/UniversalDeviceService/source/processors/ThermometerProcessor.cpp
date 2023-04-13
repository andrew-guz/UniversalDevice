#include "ThermometerProcessor.h"

#include <sstream>

#include "Logger.h"
#include "Constants.h"
#include "JsonExtension.h"
#include "DeviceInformationDescription.h"
#include "ThermometerCurrentValue.h"
#include "ExtendedThermometerCurrentValue.h"
#include "TimeHelper.h"

ThermometerProcessor::ThermometerProcessor(IQueryExecutor* queryExecutor) :
    BaseProcessorWithQueryExecutor(queryExecutor)
{

}

nlohmann::json ThermometerProcessor::ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message)
{
    if (message._header._subject == Constants::SubjectThermometerCurrentValue)
    {
        auto currentValue = JsonExtension::CreateFromJson<ThermometerCurrentValue>(message._data);
        if (currentValue._value == std::numeric_limits<float>::min())
        {
            LOG_ERROR << "ThermometerProcessor - invalid message." << std::endl;
            return {};
        }
        if (std::abs(currentValue._value - -127.0f) < 0.1f)
        {
            LOG_INFO << "-127.0 found - no sensor connected." << std::endl;
            return {};
        }
        auto& description = message._header._description;
        std::stringstream queryStream;
        queryStream
            << "INSERT INTO Thermometers (id, timestamp, value) VALUES ('"
            << description._id.data()
            << "', " 
            << TimeHelper::TimeToInt(timestamp)
            << ", '" 
            << currentValue._value
            << "')";
        queryStream.flush();
        if (!_queryExecutor->Execute(queryStream.str()))
            LOG_SQL_ERROR(queryStream.str());
        return {};
    }
    else if (message._header._subject == Constants::SubjectGetDeviceInformation)
    {
        auto description = JsonExtension::CreateFromJson<DeviceInformationDescription>(message._data);
        if (description._type == Constants::DeviceTypeThermometer &&
            !description._id.isEmpty())
        {
            std::vector<ExtendedThermometerCurrentValue> extendedThermometerCurrentValues;
            if (description._seconds != 0)
            {
                auto now = std::chrono::system_clock::now();
                now -= std::chrono::seconds(description._seconds);
                std::stringstream queryStream;
                queryStream
                    << "SELECT timestamp, value FROM Thermometers WHERE id = '"
                    << description._id.data()
                    << "' AND timestamp >= "
                    << TimeHelper::TimeToInt(now)
                    << " AND value > -126.9 ORDER BY idx DESC";
                queryStream.flush();
                std::vector<std::vector<std::string>> data;
                if (_queryExecutor->Select(queryStream.str(), data))
                    extendedThermometerCurrentValues = DbExtension::CreateVectorFromDbStrings<ExtendedThermometerCurrentValue>(data);
                else
                    LOG_SQL_ERROR(queryStream.str());
            }
            if (extendedThermometerCurrentValues.size() == 0)
            {
                std::stringstream queryStream;
                queryStream
                    << "SELECT timestamp, value FROM Thermometers WHERE id = '"
                    << description._id.data()
                    << "' AND value > -126.9 ORDER BY idx DESC LIMIT 1";
                    queryStream.flush();
                    std::vector<std::vector<std::string>> data;
                if (_queryExecutor->Select(queryStream.str(), data))
                    extendedThermometerCurrentValues = DbExtension::CreateVectorFromDbStrings<ExtendedThermometerCurrentValue>(data);
                else
                    LOG_SQL_ERROR(queryStream.str());
            }
            if (extendedThermometerCurrentValues.size())
            {
                nlohmann::json result;
                for (auto& extendedThermometerCurrentValue : extendedThermometerCurrentValues)
                    result.push_back(extendedThermometerCurrentValue.ToJson());
                return result;
            }
            else
                LOG_INFO << "No data for device " << description._id.data() << "found." << std::endl;
        }
        return {};
    }    
    return {};
}