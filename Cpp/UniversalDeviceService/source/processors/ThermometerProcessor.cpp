#include "ThermometerProcessor.h"

#include <sstream>

#include "Logger.h"
#include "Constants.h"
#include "ThermometerCurrentValue.h"
#include "ExtendedThermometerCurrentValue.h"
#include "TimeHelper.h"

ThermometerProcessor::ThermometerProcessor(IQueryExecutor* queryExecutor) :
    BaseProcessorWithQueryExecutor(queryExecutor)
{

}

nlohmann::json ThermometerProcessor::ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message)
{
    if (message._header._from._type == Constants::DeviceTypeThermometer &&
        message._header._to._type == Constants::DeviceServiceType &&
        message._header._subject == Constants::SubjectThermometerCurrentValue)
    {
        auto currentValue = ThermometerCurrentValue::CreateFromJson(message._data);
        if (currentValue._value == std::numeric_limits<float>::min())
        {
            LOG_ERROR << "ThermometerProcessor - invalid message." << std::endl;
            return {};
        }
        auto& from = message._header._from;
        std::stringstream queryStream;
        queryStream
            << "INSERT INTO Thermometers (id, timestamp, value) VALUES ('"
            << from._id.data()
            << "', '" 
            << TimeHelper::TimeToString(timestamp)
            << "', '" 
            << currentValue._value
            << "')";
        queryStream.flush();
        _queryExecutor->Execute(queryStream.str());
        return {};
    }

    if (message._header._from._type == Constants::FrontendServiceType &&
        message._header._to._type == Constants::ClientServiceType &&
        message._header._subject == Constants::SubjectGetDeviceInformation)
    {
        auto description = ComponentDescription::CreateFromJson(message._data);
        if (description._type == Constants::DeviceTypeThermometer &&
            !description._id.isEmpty())
        {
            std::stringstream queryStream;
            queryStream
                << "SELECT * FROM Thermometers WHERE id = '"
                << description._id.data()
                << "'  ORDER BY idx DESC LIMIT 100";
            queryStream.flush();
            nlohmann::json result;
            std::vector<std::vector<std::string>> data;
            if(_queryExecutor->Select(queryStream.str(), data))
            {
                auto extendedThermometerCurrentValues = ExtendedThermometerCurrentValue::CreateFromDbStrings(data);
                for (auto& extendedThermometerCurrentValue : extendedThermometerCurrentValues)
                    result.push_back(extendedThermometerCurrentValue.ToJson());
                return result;
            }
            else
            {
                LOG_ERROR << "Error in query " << queryStream.str() << "." << std::endl;
            }
        }
        return {};
    }
    
    return {};
}