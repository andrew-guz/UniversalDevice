#include "EventsService.hpp"

#include <string>

#include <crow/app.h>
#include <crow/common.h>
#include <crow/http_response.h>
#include <nlohmann/json_fwd.hpp>

#include "BaseService.hpp"
#include "Defines.hpp"
#include "Event.hpp"
#include "EventsController.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "Middleware.hpp"
#include "Uuid.hpp"

EventsService::EventsService(CrowApp& app, EventsController& eventsController) :
    _eventsController(eventsController) //
{
    CROW_ROUTE(app, API_CLIENT_EVENTS).methods(crow::HTTPMethod::GET)(ServiceExtension::bind(this, &EventsService::GetEvents));
    CROW_ROUTE(app, API_CLIENT_EVENTS).methods(crow::HTTPMethod::POST)(ServiceExtension::bindObject(this, &EventsService::AddEvent, "AddEvent"));
    CROW_ROUTE(app, API_CLIENT_EVENTS).methods(crow::HTTPMethod::PUT)(ServiceExtension::bindObject(this, &EventsService::UpdateEvent, "UpdateEvent"));
    CROW_ROUTE(app, API_CLIENT_EVENTS_ID).methods(crow::HTTPMethod::DELETE)(ServiceExtension::bind(this, &EventsService::DeleteEvent));
}

crow::response EventsService::GetEvents() const {
    nlohmann::json result;

    try {
        result = _eventsController.List();
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in EventsService::GetEvents");
        return crow::response(crow::BAD_REQUEST);
    }

    return crow::response(crow::OK, result.dump());
}

crow::response EventsService::AddEvent(Event& event) {
    nlohmann::json result;

    try {
        if (!_eventsController.Add(event)) {
            LOG_ERROR_MSG("Failed to add event");
            return crow::response(crow::BAD_REQUEST);
        }
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in EventsService::AddEvent");
        return crow::response(crow::BAD_REQUEST);
    }

    return crow::response(crow::OK);
}

crow::response EventsService::UpdateEvent(Event& event) {
    nlohmann::json result;

    try {
        if (!_eventsController.Update(event).has_value()) {
            LOG_ERROR_MSG("Failed to update event");
            return crow::response(crow::BAD_REQUEST);
        }
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in EventsService::UpdateEvent");
        return crow::response(crow::BAD_REQUEST);
    }

    return crow::response(crow::OK);
}

crow::response EventsService::DeleteEvent(const std::string& idString) {
    nlohmann::json result;

    try {
        if (!_eventsController.Delete(Uuid{ idString })) {
            LOG_ERROR_MSG("Failed to delete event");
            return crow::response(crow::BAD_REQUEST);
        }
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in EventsService::DeleteEvent");
        return crow::response(crow::BAD_REQUEST);
    }

    return crow::response(crow::OK);
}
