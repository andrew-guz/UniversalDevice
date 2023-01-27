#include "Application.h"

#include "DevicesWidget.h"

Application::Application(const Settings& settings, const Wt::WEnvironment& env) :
    Wt::WApplication(env)
{
    _mainLayout = root()->setLayout(std::make_unique<Wt::WHBoxLayout>());
    _mainStack = _mainLayout->addWidget(std::make_unique<Wt::WStackedWidget>());

    _mainStack->addWidget(std::make_unique<DevicesWidget>(settings));

    _mainStack->setCurrentIndex(0);
}
