#include "WebsocketProcessor.hpp"

#include "SimpleTableStorageCache.hpp"

WebSocketProcessor::WebSocketProcessor(IQueryExecutor* queryExecutor) : BaseProcessorWithQueryExecutor(queryExecutor) {}

nlohmann::json WebSocketProcessor::ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) {
    try {
        switch (message._header._subject) {
            case Subject::WebSocketGetSettings:
                return ProcessWebSocketGetSettingsMessage(timestamp, message);
            case Subject::WebSocketGetCommands:
                return ProcessWebSocketGetCommandsMessage(timestamp, message);
            case Subject::Undefined:
            case Subject::TimerEvent:
            case Subject::GetDeviceInformation:
            case Subject::ThermometerCurrentValue:
            case Subject::RelayCurrentState:
            case Subject::MotionRelayCurrentState:
            case Subject::WebSocketAuthorization:
                return {};
        }
    } catch (...) {
        LOG_ERROR << "Something went wrong in WebSocketProcessor::ProcessMessage." << std::endl;
        return {};
    }
    LOG_ERROR << "Unknown subject to process in WebSocketProcessor: " << static_cast<int>(message._header._subject) << std::endl;
    return {};
}

nlohmann::json WebSocketProcessor::ProcessWebSocketGetSettingsMessage(const std::chrono::system_clock::time_point& timestamp,
                                                                      const Message& message) {
    auto storageCache = SimpleTableStorageCache::GetSettingsCache(_queryExecutor);
    SimpleTableSelectInput what;
    what._id = message._header._description._id.data();
    SimpleTableSelectOutput result;
    auto problem = storageCache->Select(what, result);
    switch (problem._type) {
        case StorageCacheProblemType::NoProblems:
            return nlohmann::json::parse(result._data);
            break;
        case StorageCacheProblemType::Empty:
            LOG_INFO << "Empty settings for device " << what._id.data() << "." << std::endl;
            break;
        case StorageCacheProblemType::NotExists:
            LOG_DEBUG << "No settings for device " << what._id.data() << "." << std::endl;
            break;
        case StorageCacheProblemType::TooMany:
            LOG_ERROR << "Too many settings for device " << what._id.data() << "." << std::endl;
            break;
        case StorageCacheProblemType::SQLError:
            LOG_SQL_ERROR(problem._message);
            break;
    }
    return {};
}

nlohmann::json WebSocketProcessor::ProcessWebSocketGetCommandsMessage(const std::chrono::system_clock::time_point& timestamp,
                                                                      const Message& message) {
    auto storageCache = SimpleTableStorageCache::GetCommandsCache(_queryExecutor);
    SimpleTableSelectInput what;
    what._id = message._header._description._id.data();
    SimpleTableSelectOutput result;
    auto problem = storageCache->Select(what, result);
    switch (problem._type) {
        case StorageCacheProblemType::NoProblems:
            return nlohmann::json::parse(result._data);
            break;
        case StorageCacheProblemType::Empty:
            LOG_INFO << "Empty commands for device " << what._id.data() << "." << std::endl;
            break;
        case StorageCacheProblemType::NotExists:
            LOG_DEBUG << "No commands for device " << what._id.data() << "." << std::endl;
            break;
        case StorageCacheProblemType::TooMany:
            LOG_ERROR << "Too many commands for device " << what._id.data() << "." << std::endl;
            break;
        case StorageCacheProblemType::SQLError:
            LOG_SQL_ERROR(problem._message);
            break;
    }
    return {};
}
