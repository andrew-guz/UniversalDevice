#include "DeviceRegistrationProcessor.hpp"

#include <fmt/format.h>

#include "Marshaling.hpp"
#include "TimeHelper.hpp"

DeviceRegistrationProcessor::DeviceRegistrationProcessor(IQueryExecutor* queryExecutor) : BaseProcessorWithQueryExecutor(queryExecutor) {}

nlohmann::json DeviceRegistrationProcessor::ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) {
    auto& description = message._header._description;
    const std::string selectQuery = fmt::format("SELECT * FROM Devices WHERE id = '{}'", description._id.data());
    std::vector<std::vector<std::string>> data;
    if (_queryExecutor->Select(selectQuery, data)) {
        std::string updateInsertQuery;
        if (data.size()) {
            updateInsertQuery =
                fmt::format("UPDATE Devices SET timestamp = {} WHERE id = '{}'", TimeHelper::TimeToInt(timestamp), description._id.data());
        } else {
            updateInsertQuery = fmt::format("INSERT INTO Devices (id, type, timestamp) VALUES ('{}', '{}', {})",
                                            description._id.data(),
                                            ActorTypeToString(description._type),
                                            TimeHelper::TimeToInt(timestamp));
        }
        if (!_queryExecutor->Execute(updateInsertQuery))
            LOG_SQL_ERROR(updateInsertQuery);
    } else
        LOG_SQL_ERROR(selectQuery);
    return {};
}
