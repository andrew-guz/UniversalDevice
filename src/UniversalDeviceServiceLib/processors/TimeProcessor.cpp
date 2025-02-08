#include "TimeProcessor.hpp"

#include <nlohmann/json.hpp>

#include "TimeHelper.hpp"
#include "WebsocketsCache.hpp"

TimeProcessor::TimeProcessor(IQueryExecutor* queryExecutor) :
    BaseProcessorWithQueryExecutor(queryExecutor) {}

nlohmann::json TimeProcessor::ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) {
    const std::string dateTimeString = TimeHelper::TimeToString(timestamp);
    for (const auto& connection : WebsocketsCache::Instance()->ListWebSocketConnections()) {
        connection->send_text(nlohmann::json{
            { "date-time", dateTimeString },
        }
                                  .dump());
    }
    return {};
}
