#include "Application.h"

#include "DevicesWidget.h"

using namespace Wt;

Application::Application(const Settings& settings, const WEnvironment& env) :
    Wt::WApplication(env)
{
    _mainLayout = root()->setLayout(std::make_unique<WHBoxLayout>());
    _mainStack = _mainLayout->addWidget(std::make_unique<WStackedWidget>());

    _mainStack->addWidget(std::make_unique<DevicesWidget>(settings));

    _mainStack->setCurrentIndex(0);
}
