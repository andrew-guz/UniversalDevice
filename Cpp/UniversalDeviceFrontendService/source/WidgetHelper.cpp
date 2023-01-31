#include "WidgetHelper.h"

using namespace Wt;

void WidgetHelper::SetUsualButtonSize(WPushButton* button)
{
    button->setMinimumSize(100, 50);
    button->setMaximumSize(100, 50);
}