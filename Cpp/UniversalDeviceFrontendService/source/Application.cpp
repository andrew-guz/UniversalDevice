#include "Application.h"

#include "Logger.h"
#include "LoginWidget.h"
#include "DevicesWidget.h"
#include "ThermometerWidget.h"

using namespace Wt;

Application::Application(const Settings& settings, const WEnvironment& env) :
    Wt::WApplication(env)
{
    _mainLayout = root()->setLayout(std::make_unique<WHBoxLayout>());
    _mainStack = _mainLayout->addWidget(std::make_unique<WStackedWidget>());

    _mainStack->addWidget(std::make_unique<LoginWidget>(this, settings));
    _mainStack->addWidget(std::make_unique<DevicesWidget>(this, settings));
    _mainStack->addWidget(std::make_unique<ThermometerWidget>(this, settings));    

    _mainStack->setCurrentIndex(0);
}

void Application::SetWidget(StackWidgetType type, const std::string& data)
{
    IStackWidget* stackWidget = nullptr;
    switch (type)
    {
    case StackWidgetType::Login:
        _mainStack->setCurrentIndex(0);
        stackWidget = dynamic_cast<IStackWidget*>(_mainStack->widget(0));
        break;
    case StackWidgetType::Devices:
        _mainStack->setCurrentIndex(1);
        stackWidget = dynamic_cast<IStackWidget*>(_mainStack->widget(1));
        break;
    case StackWidgetType::Thermometer:
        _mainStack->setCurrentIndex(2);
        stackWidget = dynamic_cast<IStackWidget*>(_mainStack->widget(2));
        break;
    default:
        LOG_ERROR << "Unknown widget type " << (int)type << "." << std::endl;
        break;
    }
    if (stackWidget)
        stackWidget->Initialize(data);
}
