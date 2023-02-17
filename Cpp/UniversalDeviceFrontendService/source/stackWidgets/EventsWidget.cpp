#include "EventsWidget.h"

#include "Defines.h"
#include "Constants.h"
#include "Logger.h"
#include "MessageHelper.h"
#include "RequestHelper.h"
#include "WidgetHelper.h"

using namespace Wt;

EventsWidget::EventsWidget(IStackHolder* stackHolder, const Settings& settings) :
    BaseStackWidget(stackHolder, settings)
{
    _mainLayout = setLayout(std::make_unique<WGridLayout>());
    _mainLayout->addWidget(std::make_unique<WText>("Список известных устройств:"), 0, 0, 0, 4, AlignmentFlag::Left);

    Refresh();
}

void EventsWidget::Initialize(const std::string& data)
{
    Refresh();
}

void EventsWidget::Clear()
{
}

void EventsWidget::Refresh()
{
    Clear();
}
