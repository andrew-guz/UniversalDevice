#include "WebsocketProcessor.h"

#include "StorageCacheFactory.h"
#include "SimpleTableStorageCache.h"

WebSocketProcessor::WebSocketProcessor(IQueryExecutor* queryExecutor) :
    BaseProcessorWithQueryExecutor(queryExecutor)
{

}

nlohmann::json WebSocketProcessor::ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message)
{
    try
    {
        if (message._header._subject == Constants::SubjectWebSocketGetSettings)
        {
            auto storageCache = StorageCacheFactory::Instance()->GetStorageCache<SimpleTableStorageCache>(_queryExecutor, "Settings", "settings");
            SimpleTableSelectInput what;
            what._id = message._header._description._id.data();
            SimpleTableSelectOutput result;
            auto problem = storageCache->Select(what, result);
            switch(problem._type)
            {
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
        }
        else if (message._header._subject == Constants::SubjectWebSocketGetCommands)
        {
            auto storageCache = StorageCacheFactory::Instance()->GetStorageCache<SimpleTableStorageCache>(_queryExecutor, "Commands", "commands");
            SimpleTableSelectInput what;
            what._id = message._header._description._id.data();;
            SimpleTableSelectOutput result;
            auto problem = storageCache->Select(what, result);
            switch(problem._type)
            {
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
        }
    }
    catch(...)
    {
        LOG_ERROR << "Something went wrong in WebSocketProcessor::ProcessMessage." << std::endl;
    }
    return nlohmann::json();
}
