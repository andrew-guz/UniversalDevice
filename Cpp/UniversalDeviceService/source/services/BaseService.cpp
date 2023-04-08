#include "BaseService.h"

#include <vector>

#include "JsonExtension.h"
#include "AccountManager.h"
#include "ProcessorsFactory.h"

BaseService::BaseService(IQueryExecutor* queryExecutor) :
    _queryExecutor(queryExecutor)
{
    
}

bool BaseService::IsValidUser(const crow::request& request)
{
    auto authorization = request.get_header_value("Authorization");
    if (authorization.empty())
        return false;
    return AccountManager::Instance()->IsValidUser(authorization);
}

void BaseService::CallProcessorsNoResult(const std::chrono::system_clock::time_point& timestamp, const Message& message)
{
    auto processors = ProcessorsFactory::CreateProcessors(message, _queryExecutor);
    for (auto& processor : processors)
        processor->ProcessMessage(timestamp, message);
}

nlohmann::json BaseService::CallProcessorsJsonResult(const std::chrono::system_clock::time_point& timestamp, const Message& message)
{
    nlohmann::json result;
    std::vector<nlohmann::json> processorJsonResults;
    auto processors = ProcessorsFactory::CreateProcessors(message, _queryExecutor);
    for (auto& processor : processors)
    {
        auto processorResultJson = processor->ProcessMessage(timestamp, message);
        if (processorResultJson.is_null())
            continue;
        processorJsonResults.push_back(processorResultJson);
    }
    if (processorJsonResults.size() == 1)
        result = processorJsonResults[0];
    else if (processorJsonResults.size() > 1)
    {
        for (auto& processorResultJson : processorJsonResults)
        {
            if (processorResultJson.is_array())
            {
                for (auto& subResult : processorResultJson)
                    result.push_back(subResult);
            }
            else
                result.push_back(processorResultJson);
        }
    }
    return result;
}

Message BaseServiceExtension::GetMessageFromRequest(const crow::request& request)
{
    auto body = request.body;
    try
    {           
        auto bodyJson = nlohmann::json::parse(body);
        LOG_DEBUG << bodyJson.dump() << std::endl;
        return JsonExtension::CreateFromJson<Message>(bodyJson);
    }
    catch(...)
    {
        LOG_ERROR << "Can't get message from request - " << body << std::endl;
    }
    return Message();        
}
