#include "Application.h"

#include <Wt/WEnvironment.h>

#include "DevicesWidget.h"
#include "EventsWidget.h"
#include "Logger.h"
#include "LoginWidget.h"
#include "LogsWidget.h"
#include "MotionRelayWidget.h"
#include "RelayWidget.h"
#include "ThermometerWidget.h"

using namespace Wt;

Application::Application(const Settings& settings, const WEnvironment& env) : Wt::WApplication(env) {
    setCssTheme("polished");

    _mainLayout = root()->setLayout(std::make_unique<WHBoxLayout>());
    _mainStack = _mainLayout->addWidget(std::make_unique<WStackedWidget>());

    _mainStack->addWidget(std::make_unique<LoginWidget>(this, settings));
    _mainStack->addWidget(std::make_unique<DevicesWidget>(this, settings));
    _mainStack->addWidget(std::make_unique<EventsWidget>(this, settings));
    _mainStack->addWidget(std::make_unique<LogsWidget>(this, settings));
    _mainStack->addWidget(std::make_unique<ThermometerWidget>(this, settings));
    _mainStack->addWidget(std::make_unique<RelayWidget>(this, settings));
    _mainStack->addWidget(std::make_unique<MotionRelayWidget>(this, settings));

    auto authorization = env.getCookie("authorization");
    SetWidget(StackWidgetType::Login, authorization ? *authorization : std::string{});
}

void Application::SetWidget(StackWidgetType type, const std::string& data) {
    IStackWidget* stackWidget = nullptr;
    switch (type) {
        case StackWidgetType::Login:
            _mainStack->setCurrentIndex(0);
            stackWidget = dynamic_cast<IStackWidget*>(_mainStack->widget(0));
            break;
        case StackWidgetType::Devices:
            _mainStack->setCurrentIndex(1);
            stackWidget = dynamic_cast<IStackWidget*>(_mainStack->widget(1));
            break;
        case StackWidgetType::Events:
            _mainStack->setCurrentIndex(2);
            stackWidget = dynamic_cast<IStackWidget*>(_mainStack->widget(2));
            break;
        case StackWidgetType::Logs:
            _mainStack->setCurrentIndex(3);
            stackWidget = dynamic_cast<IStackWidget*>(_mainStack->widget(3));
            break;
        case StackWidgetType::Thermometer:
            _mainStack->setCurrentIndex(4);
            stackWidget = dynamic_cast<IStackWidget*>(_mainStack->widget(4));
            break;
        case StackWidgetType::Relay:
            _mainStack->setCurrentIndex(5);
            stackWidget = dynamic_cast<IStackWidget*>(_mainStack->widget(5));
            break;
        case StackWidgetType::MotionRelay:
            _mainStack->setCurrentIndex(6);
            stackWidget = dynamic_cast<IStackWidget*>(_mainStack->widget(6));
            break;
        default:
            LOG_ERROR << "Unknown widget type " << (int)type << "." << std::endl;
            break;
    }
    if (stackWidget)
        stackWidget->Initialize(data);
}
