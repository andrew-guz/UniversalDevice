#include "DeviceService.h"

#include "Defines.h"
#include "Logger.h"
#include "TimeHelper.h"
#include "ProcessorsFactory.h"

DeviceService::DeviceService(IQueryExecutor* queryExecutor) :
    BaseService(queryExecutor)
{

}

void DeviceService::Initialize(crow::SimpleApp& app)
{
    CROW_ROUTE(app, API_DEVICE_INFORM).methods(crow::HTTPMethod::POST)([&](const crow::request& request){ return Inform(request); });
}

int DeviceService::Inform(const crow::request& request)
{
    try
    {
        auto timestamp = std::chrono::system_clock::now();
        auto message = GetMessageFromRequest(request);
        if (!message.IsValid())
        {
            LOG_ERROR << "Invalid message." << std::endl;
            return crow::BAD_REQUEST;
        }
        auto processors = ProcessorsFactory::CreateProcessors(message, _queryExecutor);
        for (auto& processor : processors)
            processor->ProcessMessage(timestamp, message);
    }
    catch(...)
    {
        return crow::BAD_REQUEST;
    }    
    return crow::OK;
}
