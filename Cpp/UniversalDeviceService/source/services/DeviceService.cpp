#include "DeviceService.h"

#include "Defines.h"
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
    auto body = request.body;
    try
    {
        auto timestamp = std::chrono::system_clock::now();
        auto body_json = nlohmann::json::parse(body);
        std::cout << "Inform [" << TimeHelper::TimeToString(timestamp) << "]:  " << body_json.dump() << std::endl;
        auto message = Message::CreateFromJson(body_json);
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
