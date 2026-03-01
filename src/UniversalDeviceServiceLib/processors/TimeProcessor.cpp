#include "TimeProcessor.hpp"

#include <chrono>
#include <string>

#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include "CleanupController.hpp"
#include "Message.hpp"
#include "TimeHelper.hpp"
#include "WebsocketsCache.hpp"

TimeProcessor::TimeProcessor(CleanupController& cleanupController) :
    _cleanupController(cleanupController) {}

nlohmann::json TimeProcessor::ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) {
    // Notify all devices about current time
    const std::string dateTimeString = TimeHelper::TimeToString(timestamp);
    const auto [hour, minute, day, month, year] = TimeHelper::GetHourMinuteDayMonthYear(timestamp);
    for (const auto& connection : WebsocketsCache::Instance()->ListWebSocketConnections()) {
        connection->send_text(nlohmann::json{
            { "date-time", dateTimeString },
            { "hour", hour },
            { "minute", minute },
            { "day", day },
            { "month", month },
            { "year", year },
        }
                                  .dump());
    }

    // Maybe it's time to cleanup database
    _cleanupController.Cleanup(timestamp);

    return {};
}
