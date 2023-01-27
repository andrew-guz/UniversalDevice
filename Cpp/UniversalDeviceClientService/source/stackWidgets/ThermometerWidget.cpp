#include "ThermometerWidget.h"

#include "Defines.h"
#include "Logger.h"
#include "RequestHelper.h"

using namespace Wt;

ThermometerWidget::ThermometerWidget(IStackHolder* stackHolder, const Settings& settings) :
    BaseStackWidget(stackHolder, settings)
{
    _mainLayout = setLayout(std::make_unique<WGridLayout>());
}
