#include "BaseService.hpp"

#include <fmt/format.h>

#include "Marshaling.hpp"
#include "ProcessorsFactory.hpp"

BaseService::BaseService(IQueryExecutor* queryExecutor) : _queryExecutor(queryExecutor) {}

void BaseService::CallProcessorsNoResult(const std::chrono::system_clock::time_point& timestamp, const Message& message) {
    auto processors = ProcessorsFactory::CreateProcessors(message, _queryExecutor);
    for (auto& processor : processors)
        processor->ProcessMessage(timestamp, message);
}

nlohmann::json BaseService::CallProcessorsJsonResult(const std::chrono::system_clock::time_point& timestamp, const Message& message) {
    nlohmann::json result;
    std::vector<nlohmann::json> processorJsonResults;
    auto processors = ProcessorsFactory::CreateProcessors(message, _queryExecutor);
    for (auto& processor : processors) {
        auto processorResultJson = processor->ProcessMessage(timestamp, message);
        if (processorResultJson.is_null())
            continue;
        processorJsonResults.push_back(processorResultJson);
    }
    if (processorJsonResults.size() == 1)
        result = processorJsonResults[0];
    else if (processorJsonResults.size() > 1) {
        for (auto& processorResultJson : processorJsonResults) {
            if (processorResultJson.is_array()) {
                for (auto& subResult : processorResultJson)
                    result.push_back(subResult);
            } else
                result.push_back(processorResultJson);
        }
    }
    return result;
}

Message BaseServiceExtension::GetMessageFromRequest(const crow::request& request) {
    auto body = request.body;
    try {
        auto bodyJson = nlohmann::json::parse(body);
        LOG_DEBUG_MSG(bodyJson.dump());
        return bodyJson.get<Message>();
    } catch (...) {
        LOG_ERROR_MSG(fmt::format("Can't get message from request - {}", body));
    }
    return Message();
}

Message BaseServiceExtension::GetMessageFromWebSocketData(const std::string& data) {
    try {
        auto dataJson = nlohmann::json::parse(data);
        LOG_DEBUG_MSG(dataJson.dump());
        return dataJson.get<Message>();
    } catch (...) {
        LOG_ERROR_MSG(fmt::format("Can't get message from data - {}", data));
    }
    return Message();
}
