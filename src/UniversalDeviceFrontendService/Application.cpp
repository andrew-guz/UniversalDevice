#include "Application.hpp"

#include <Wt/WEnvironment.h>
#include <Wt/WGlobal.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WText.h>
#include <fmt/format.h>

#include "DevicesWidget.hpp"
#include "EventsWidget.hpp"
#include "Logger.hpp"
#include "LoginWidget.hpp"
#include "LogsWidget.hpp"
#include "MotionRelayWidget.hpp"
#include "RelayWidget.hpp"
#include "ScenariosWidget.hpp"
#include "ThermometerWidget.hpp"
#include "UniversalDeviceWidget.hpp"
#include "Version.hpp"

using namespace Wt;

Application::Application(const Settings& settings, const WEnvironment& env) :
    Wt::WApplication(env) {
    setTitle(fmt::format("Universal device [{}.{}.{}]", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH));
    setCssTheme("polished");

    _mainLayout = root()->setLayout(std::make_unique<WHBoxLayout>());
    _mainStack = _mainLayout->addWidget(std::make_unique<WStackedWidget>());

    _mainStack->addWidget(std::make_unique<LoginWidget>(this, settings));
    _mainStack->addWidget(std::make_unique<DevicesWidget>(this, settings));
    _mainStack->addWidget(std::make_unique<EventsWidget>(this, settings));
    _mainStack->addWidget(std::make_unique<ScenariosWidget>(this, settings));
    _mainStack->addWidget(std::make_unique<LogsWidget>(this, settings));
    _mainStack->addWidget(std::make_unique<ThermometerWidget>(this, settings));
    _mainStack->addWidget(std::make_unique<RelayWidget>(this, settings));
    _mainStack->addWidget(std::make_unique<MotionRelayWidget>(this, settings));
    _mainStack->addWidget(std::make_unique<UniversalDeviceWidget>(this, settings));

    auto authorization = env.getCookie("authorization");
    SetWidget(StackWidgetType::Login, authorization ? *authorization : std::string{});
}

void Application::SetWidget(StackWidgetType type, const std::string& data) {
    IStackWidget* stackWidget = nullptr;
    switch (type) {
        case StackWidgetType::Login:
        case StackWidgetType::Devices:
        case StackWidgetType::Events:
        case StackWidgetType::Scenarios:
        case StackWidgetType::Logs:
        case StackWidgetType::Thermometer:
        case StackWidgetType::Relay:
        case StackWidgetType::MotionRelay:
        case StackWidgetType::UniversalDevice:
            const int index = static_cast<int>(type);
            _mainStack->setCurrentIndex(index);
            stackWidget = dynamic_cast<IStackWidget*>(_mainStack->widget(index));
            break;
    }
    if (stackWidget)
        stackWidget->Initialize(data);
    else
        LOG_ERROR_MSG(fmt::format("Unknown widget type: {}", static_cast<int>(type)));
}
