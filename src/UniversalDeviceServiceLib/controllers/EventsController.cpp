#include "EventsController.hpp"

#include <algorithm>
#include <mutex>
#include <optional>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include <boost/hof/first_of.hpp>
#include <boost/hof/match.hpp>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include "Controller.hpp"
#include "DbExtension.hpp"
#include "Event.hpp"
#include "EventUtils.hpp"
#include "IQueryExecutor.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "Provider.hpp"
#include "RelayEvent.hpp"

EventsController::EventsController(IQueryExecutor* queryExecutor) :
    Controller(queryExecutor) //
{
    FillCache();
}

std::vector<Event> EventsController::List() const {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    if (_cache.empty())
        FillCache();

    return _cache;
}

std::vector<Event> EventsController::GetByProvider(const Uuid& providerId) const {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    if (_cache.empty())
        FillCache();

    std::vector<Event> result;
    result.reserve(_cache.size());
    for (const Event& event : _cache) {
        if (std::visit([&providerId](const auto& e) -> bool { return GetProviderId(e._provider) == providerId && e._active; }, event)) {
            result.emplace_back(event);
        }
    }

    return result;
}

std::optional<Event> EventsController::GetById(const Uuid& id) const {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    if (_cache.empty())
        FillCache();

    auto iter = std::find_if(_cache.begin(), _cache.end(), [&id](const Event& event) -> bool { return GetEventId(event) == id; });
    if (iter != _cache.end())
        return *iter;

    return std::nullopt;
}

bool EventsController::Add(const Event& event) {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    if (_cache.empty())
        FillCache();

    const std::string eventId = GetEventId(event).data();
    const std::string eventJson = static_cast<nlohmann::json>(event).dump();
    const std::string query = fmt::format("INSERT INTO Events (id, event) VALUES ('{}', '{}')", eventId, eventJson);

    if (_queryExecutor->Execute(query)) {
        _cache.emplace_back(std::move(event));
        return true;
    }

    LOG_SQL_ERROR(query);

    return false;
}

std::optional<Event> EventsController::Update(Event& event) {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    if (_cache.empty())
        FillCache();

    const Uuid id = GetEventId(event);
    auto iter = std::find_if(_cache.begin(), _cache.end(), [&id](const Event& event) -> bool { return GetEventId(event) == id; });

    if (iter == _cache.end()) {
        LOG_ERROR_MSG("Event not found");
        return std::nullopt;
    }

    return UpdateImpl(event);
}

std::optional<Event> EventsController::SetActivity(const Uuid& eventId, const bool activity) {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    if (_cache.empty())
        FillCache();

    auto iter = std::find_if(_cache.begin(), _cache.end(), [&eventId](const Event& event) -> bool { return GetEventId(event) == eventId; });

    if (iter == _cache.end()) {
        LOG_ERROR_MSG("Event not found");
        return std::nullopt;
    }

    Event event = *iter;

    std::visit([activity](auto& e) { e._active = activity; }, event);

    return UpdateImpl(event);
}

bool EventsController::Delete(const Uuid& eventId) {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    auto iter = std::find_if(_cache.begin(), _cache.end(), [&eventId](const Event& event) -> bool { return GetEventId(event) == eventId; });

    if (iter == _cache.end()) {
        LOG_ERROR_MSG("Event not found");
        return false;
    }

    const std::string query = fmt::format("DELETE FROM Events WHERE id='{}'", eventId.data());
    if (_queryExecutor->Execute(query)) {
        FillCache(); // due to I can't operator= for variant, I can't remove from vector and have to reread it
        return true;
    }

    LOG_SQL_ERROR(query);

    return false;
}

void EventsController::FillCache() const {
    const std::string query = "SELECT * FROM Events";
    std::vector<std::vector<std::string>> data;
    if (_queryExecutor->Select(query, data)) {
        _cache = DbExtension::CreateVectorFromDbStrings<Event>(data);
    } else
        LOG_SQL_ERROR(query);
}

std::optional<Event> EventsController::UpdateImpl(Event& event) {
    const std::string eventJson = static_cast<nlohmann::json>(event).dump();
    const Uuid eventId = GetEventId(event);
    const std::string query = fmt::format("UPDATE Events SET event = '{}' WHERE id = '{}'", eventJson, eventId.data());

    if (_queryExecutor->Execute(query)) {
        auto iter = std::find_if(_cache.begin(), _cache.end(), [&eventId](const Event& event) -> bool { return GetEventId(event) == eventId; });

        if (iter == _cache.end()) {
            LOG_ERROR_MSG("WTF??? Event exists in database, but not in cache!");
            return std::nullopt;
        }

        Event& currentEvent = *iter;
        std::visit(boost::hof::match([&currentEvent](TimerEvent& e) { currentEvent.emplace<TimerEvent>(e); },
                                     [&currentEvent](ThermometerEvent& e) { currentEvent.emplace<ThermometerEvent>(e); },
                                     [&currentEvent](RelayEvent& e) { currentEvent.emplace<RelayEvent>(e); },
                                     [&currentEvent](ThermostatEvent& e) { currentEvent.emplace<ThermostatEvent>(e); },
                                     [&currentEvent](SunriseEvent& e) { currentEvent.emplace<SunriseEvent>(e); },
                                     [&currentEvent](SunsetEvent& e) { currentEvent.emplace<SunsetEvent>(e); }),
                   event);
        return event;
    }

    LOG_SQL_ERROR(query);

    return std::nullopt;
}
