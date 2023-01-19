#include "ThermometerProcessor.h"

ThermometerProcessor::ThermometerProcessor(IQueryExecutor* queryExecutor) :
    BaseProcessorWithQueryExecutor(queryExecutor)
{

}

void ThermometerProcessor::ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message &message)
{
    //TODO
}