#pragma once

#include <optional>
#include <vector>

#include "Controller.hpp"
#include "Event.hpp"
#include "IQueryExecutor.hpp"
#include "Uuid.hpp"

class EventsController final : public Controller {
public:
    EventsController(IQueryExecutor* queryExecutor);

    ~EventsController() = default;

    std::vector<Event> List() const;

    std::vector<Event> GetByProvider(const Uuid& providerId) const;

    std::optional<Event> GetById(const Uuid& id) const;

    bool Add(const Event& event);

    std::optional<Event> Update(Event& event);

    std::optional<Event> SetActivity(const Uuid& eventId, bool activity);

    bool Delete(const Uuid& eventId);

private:
    void FillCache() const;

    std::optional<Event> UpdateImpl(Event& event);

private:
    mutable std::vector<Event> _cache;
};
