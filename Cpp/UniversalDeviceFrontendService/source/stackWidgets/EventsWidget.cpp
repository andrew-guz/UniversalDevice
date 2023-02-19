#include "EventsWidget.h"

#include <Wt/WText.h>
#include <Wt/WCheckBox.h>

#include "Defines.h"
#include "Constants.h"
#include "Logger.h"
#include "MessageHelper.h"
#include "UrlHelper.h"
#include "RequestHelper.h"
#include "WidgetHelper.h"
#include "Event.h"
#include "TimerEvent.h"
#include "ThermometerEvent.h"
#include "RelayEvent.h"

using namespace Wt;

EventsWidget::EventsWidget(IStackHolder* stackHolder, const Settings& settings) :
    BaseStackWidget(stackHolder, settings)
{
    _mainLayout = setLayout(std::make_unique<WGridLayout>());

    auto backButton = _mainLayout->addWidget(std::make_unique<WPushButton>("Назад..."), 0, 0, 1, 2, AlignmentFlag::Left);
    WidgetHelper::SetUsualButtonSize(backButton);
    backButton->clicked().connect([&](){
        _stackHolder->SetWidget(StackWidgetType::Devices, "");
    });

    Refresh();
}

void EventsWidget::Initialize(const std::string& data)
{
    Refresh();
}

void EventsWidget::Clear()
{
    _eventJsons.clear();
}

void EventsWidget::Refresh()
{
    Clear();

    _eventJsons = GetEvents();
}

std::vector<nlohmann::json> EventsWidget::GetEvents()
{
    return RequestHelper::DoGetRequest({ "127.0.0.1", _settings._servicePort, API_CLIENT_EVENTS }, Constants::LoginService);
}
