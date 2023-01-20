#include "ThermometerProcessor.h"

#include <iostream>
#include <sstream>

#include "Constants.h"
#include "ThermometerCurrentValue.h"
#include "TimeHelper.h"

ThermometerProcessor::ThermometerProcessor(IQueryExecutor* queryExecutor) :
    BaseProcessorWithQueryExecutor(queryExecutor)
{

}

void ThermometerProcessor::ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message)
{
    if (message._header._subject == Constants::SubjectThermometerCurrentValue)
    {
        auto currentValue = ThermometerCurrentValue::CreateFromJson(message._data);
        if (currentValue._value == std::numeric_limits<float>::min())
        {
            std::cout << "ThermometerProcessor - invalid message" << std::endl;
            return; 
        }
        std::stringstream queryStream;
        queryStream
            << "INSERT INTO 'Thermometers' ('id', 'timestamp', 'value') VALUES ('"
            << message._header._id.data()
            << "', '" 
            << TimeHelper::TimeToString(timestamp)
            << "', '" 
            << currentValue._value
            << "')";
        queryStream.flush();
        _queryExecutor->Execute(queryStream.str());
    }
}