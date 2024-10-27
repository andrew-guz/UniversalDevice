#include "EventsService.hpp"

#include "Event.hpp"
#include "EventTableStorageCache.hpp"
#include "Marshaling.hpp"

EventsService::EventsService(IQueryExecutor* queryExecutor) :
    BaseService(queryExecutor) {}

void EventsService::Initialize(CrowApp& app) {
    CROW_ROUTE(app, API_CLIENT_EVENTS).methods(crow::HTTPMethod::GET)(BaseService::bind(this, &EventsService::GetEvents));
    CROW_ROUTE(app, API_CLIENT_EVENTS).methods(crow::HTTPMethod::POST)(BaseService::bindObject(this, &EventsService::AddEvent, "AddEvent"));
    CROW_ROUTE(app, API_CLIENT_EVENTS).methods(crow::HTTPMethod::PUT)(BaseService::bindObject(this, &EventsService::UpdateEvent, "UpdateEvent"));
    CROW_ROUTE(app, API_CLIENT_EVENTS).methods(crow::HTTPMethod::DELETE)(BaseService::bindObject(this, &EventsService::DeleteEvent, "DeleteEvent"));
}

crow::response EventsService::GetEvents() const {
    nlohmann::json result = nlohmann::json::array({});
    try {
        std::vector<std::string> eventStrings;

        auto storageCache = EventTableStorageCache::GetCache(_queryExecutor);
        EventTableSelectAllOutput eventsResult;
        auto problem = storageCache->SelectAll(eventsResult);
        switch (problem._type) {
            case StorageCacheProblemType::NoProblems:
                eventStrings = eventsResult._data;
                break;
            case StorageCacheProblemType::Empty:
            case StorageCacheProblemType::NotExists:
            case StorageCacheProblemType::TooMany:
                break;
            case StorageCacheProblemType::SQLError:
                LOG_SQL_ERROR(problem._message);
                break;
        }

        for (auto& eventString : eventStrings) {
            try {
                nlohmann::json eventJson = nlohmann::json::parse(eventString);
                result.push_back(eventJson);
            } catch (...) {
                LOG_ERROR_MSG(fmt::format("Invalid event JSON {}", eventString));
            }
        }
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in ClientService::GetEvents");
    }
    return crow::response(crow::OK, result.dump());
}

crow::response EventsService::AddEvent(const Event& event, const std::string& eventString) {
    auto storageCache = EventTableStorageCache::GetCache(_queryExecutor);
    EventTableInsertOrReplaceInput what{
        ._id = event._id,
        ._active = event._active,
        ._providerId = event._provider._id,
        ._providerType = event._provider._type,
        ._event = eventString,
    };
    auto problem = storageCache->InsertOrReplace(what);
    switch (problem._type) {
        case StorageCacheProblemType::NoProblems:
            return crow::response(crow::OK);
            break;
        case StorageCacheProblemType::Empty:
        case StorageCacheProblemType::NotExists:
        case StorageCacheProblemType::TooMany:
            break;
        case StorageCacheProblemType::SQLError:
            LOG_SQL_ERROR(problem._message);
            break;
    }
    return crow::response(crow::BAD_REQUEST);
}

crow::response EventsService::UpdateEvent(const Event& event, const std::string& eventString) {
    auto storageCache = EventTableStorageCache::GetCache(_queryExecutor);
    EventTableUpdateInput what{
        ._id = event._id,
        ._active = event._active,
        ._providerId = event._provider._id,
        ._providerType = event._provider._type,
        ._event = eventString,
    };
    auto problem = storageCache->Update(what);
    switch (problem._type) {
        case StorageCacheProblemType::NoProblems:
            return crow::response(crow::OK);
            break;
        case StorageCacheProblemType::Empty:
        case StorageCacheProblemType::NotExists:
        case StorageCacheProblemType::TooMany:
            break;
        case StorageCacheProblemType::SQLError:
            LOG_SQL_ERROR(problem._message);
            break;
    }
    return crow::response(crow::BAD_REQUEST);
}

crow::response EventsService::DeleteEvent(const Event& event) {
    auto storageCache = EventTableStorageCache::GetCache(_queryExecutor);
    EventTableDeleteInput what{
        ._id = event._id,
    };
    auto problem = storageCache->Delete(what);
    switch (problem._type) {
        case StorageCacheProblemType::NoProblems:
            return crow::response(crow::OK);
            break;
        case StorageCacheProblemType::Empty:
        case StorageCacheProblemType::NotExists:
        case StorageCacheProblemType::TooMany:
            break;
        case StorageCacheProblemType::SQLError:
            LOG_SQL_ERROR(problem._message);
            break;
    }
    return crow::response(crow::BAD_REQUEST);
}
