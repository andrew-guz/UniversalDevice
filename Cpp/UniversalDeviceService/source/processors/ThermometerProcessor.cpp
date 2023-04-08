#include "ThermometerProcessor.h"

#include <sstream>

#include "JsonExtension.h"
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
    if (message._header._subject == Constants::SubjectThermometerCurrentValue)
    {
        auto currentValue = JsonExtension::CreateFromJson<ThermometerCurrentValue>(message._data);
        if (currentValue._value == std::numeric_limits<float>::min())
        {
            LOG_ERROR << "ThermometerProcessor - invalid message." << std::endl;
            return {};
        }
        auto& description = message._header._description;
        std::stringstream queryStream;
        queryStream
            << "INSERT INTO Thermometers (id, timestamp, value) VALUES ('"
            << description._id.data()
            << "', '" 
            << TimeHelper::TimeToString(timestamp)
            << "', '" 
            << currentValue._value
            << "')";
        queryStream.flush();
        if (!_queryExecutor->Execute(queryStream.str()))
            LOG_SQL_ERROR(queryStream.str());
        return {};
    }
    else if (message._header._subject == Constants::SubjectGetDeviceInformationSingle ||
             message._header._subject == Constants::SubjectGetDeviceInformationMultiple)
    {
        auto description = JsonExtension::CreateFromJson<ComponentDescription>(message._data);
        if (description._type == Constants::DeviceTypeThermometer &&
            !description._id.isEmpty())
        {
            std::stringstream queryStream;
            queryStream
                << "SELECT timestamp, value FROM Thermometers WHERE id = '"
                << description._id.data()
                << "' ORDER BY idx DESC LIMIT "
                << (message._header._subject == Constants::SubjectGetDeviceInformationSingle ? "1" : "120");
            queryStream.flush();
            nlohmann::json result;
            std::vector<std::vector<std::string>> data;
            if (_queryExecutor->Select(queryStream.str(), data))
            {
                auto extendedThermometerCurrentValues = DbExtension::CreateVectorFromDbStrings<ExtendedThermometerCurrentValue>(data);
                for (auto& extendedThermometerCurrentValue : extendedThermometerCurrentValues)
                    result.push_back(extendedThermometerCurrentValue.ToJson());
                return result;
            }
            else
                LOG_SQL_ERROR(queryStream.str());
        }
        return {};
    }
    
    return {};
}