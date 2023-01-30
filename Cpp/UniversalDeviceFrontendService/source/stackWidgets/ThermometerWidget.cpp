#include "ThermometerWidget.h"

#include <Wt/WPushButton.h>

#include "Defines.h"
#include "Logger.h"
#include "RequestHelper.h"

using namespace Wt;

ThermometerWidget::ThermometerWidget(IStackHolder* stackHolder, const Settings& settings) :
    BaseStackWidget(stackHolder, settings)
{
    _mainLayout = setLayout(std::make_unique<WGridLayout>());
    auto backButton = _mainLayout->addWidget(std::make_unique<WPushButton>(), 0, 2, AlignmentFlag::Right);
    backButton->setText("Назад...");
    backButton->clicked().connect([&](){ _stackHolder->SetWidget(StackWidgetType::Devices, ""); });
}

void ThermometerWidget::Initialize(const std::string& data)
{

}
