#include "ScenarioButton.hpp"

#include <fmt/format.h>

#include "Defines.hpp"
#include "Event.hpp"
#include "FrontendDefines.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "RequestHelper.hpp"
#include "WidgetHelper.hpp"

using namespace Wt;

ScenarioButton::ScenarioButton(int port, Scenario scenario) :
    WPushButton(),
    _port(port),
    _scenario(std::move(scenario)) {
    setText(_scenario._name);
    WidgetHelper::SetUsualButtonSize(this);

    clicked().connect([this]() {
        const auto result = RequestHelper::DoPatchRequest(
            { BACKEND_IP, _port, fmt::format("{}/{}", API_CLIENT_SCENARIOS, _scenario._id.data()) }, Constants::LoginService, {});
        if (result != 200) {
            LOG_ERROR_MSG(fmt::format("Error while activating Scenario {}", _scenario._name));
            WidgetHelper::ShowSimpleMessage(this, "Ошибка", "Ошибка активации сценария!");
        } else
            WidgetHelper::ShowSimpleMessage(this, "Информация", "Сценарий активирован!", 5000);
        Refresh();
    });

    _refreshTimer = addChild(std::make_unique<WTimer>());
    _refreshTimer->setInterval(std::chrono::seconds(5));
    _refreshTimer->timeout().connect([this]() { Refresh(); });
    _refreshTimer->start();
    Refresh();
}

ScenarioButton::~ScenarioButton() { _refreshTimer->stop(); }

void ScenarioButton::Refresh() {
    const auto eventsReplyJson = RequestHelper::DoGetRequest({ BACKEND_IP, _port, API_CLIENT_EVENTS }, Constants::LoginService);
    const auto events = !eventsReplyJson.is_null() ? eventsReplyJson.get<std::vector<Event>>() : std::vector<Event>{};

    bool allEventsAreOK = true;
    for (const Uuid& activeEvent : _scenario._activateEvent) {
        if (!allEventsAreOK)
            break;
        const auto iter = std::find_if(events.begin(), events.end(), [&activeEvent](const Event& event) { return event._id == activeEvent; });
        if (iter == events.end()) {
            allEventsAreOK = false;
            break;
        }
        allEventsAreOK &= iter->_active;
    }
    for (const Uuid& inactiveEvent : _scenario._deactivateEvent) {
        if (!allEventsAreOK)
            break;
        const auto iter = std::find_if(events.begin(), events.end(), [&inactiveEvent](const Event& event) { return event._id == inactiveEvent; });
        if (iter == events.end()) {
            allEventsAreOK = false;
            break;
        }
        allEventsAreOK &= !iter->_active;
    }

    if (allEventsAreOK)
        decorationStyle().setBorder(WBorder(Wt::BorderStyle::Solid, 2, Wt::StandardColor::DarkGreen));
    else
        decorationStyle().setBorder(WBorder(Wt::BorderStyle::Solid, 1, Wt::StandardColor::DarkGray));
}
