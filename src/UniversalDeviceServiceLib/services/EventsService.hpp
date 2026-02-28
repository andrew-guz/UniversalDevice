#pragma once

#include <string>

#include <crow/http_response.h>

#include "Event.hpp"
#include "EventsController.hpp"
#include "Middleware.hpp"

class EventsService final {
public:
    EventsService(CrowApp& app, EventsController& eventsController);

    virtual ~EventsService() = default;

private:
    crow::response GetEvents() const;

    crow::response AddEvent(Event& event);

    crow::response UpdateEvent(Event& event);

    crow::response DeleteEvent(const std::string& id);

private:
    EventsController& _eventsController;

private:
    friend class ServiceExtension;
};
