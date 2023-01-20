#include "ThermometerProcessor.h"

#include <iostream>
#include <sstream>

#include "Constants.h"
#include "TimeHelper.h"

ThermometerProcessor::ThermometerProcessor(IQueryExecutor* queryExecutor) :
    BaseProcessorWithQueryExecutor(queryExecutor)
{

}

void ThermometerProcessor::ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message)
{
    if (!message._data.contains("value"))
    {
        std::cout << "ThermometerProcessor - invalid message" << std::endl;
        return; 
    }
    if (message._header._subject == Constants::SubjectThermometerCurrentValue)
    {
        std::stringstream queryStream;
        queryStream
            << "INSERT INTO 'Thermometers' ('id', 'timestamp', 'value') VALUES ('"
            << message._header._id.data()
            << "', '" 
            << TimeHelper::TimeToString(timestamp)
            << "', '" 
            << message._data["value"].get<float>()
            << "')";
        queryStream.flush();
        _queryExecutor->Execute(queryStream.str());
    }
}