#ifndef _THERMOMETER_WIDGET_H_
#define _THERMOMETER_WIDGET_H_

#include <Wt/WContainerWidget.h>
#include <Wt/WGridLayout.h>

#include "BaseStackWidget.h"

class ThermometerWidget final : public Wt::WContainerWidget, public BaseStackWidget
{
public:
    ThermometerWidget(IStackHolder* stackHolder, const Settings& settings);

    virtual ~ThermometerWidget() = default;

    virtual void Initialize(const std::string& data) override;

private:
    Wt::WGridLayout* _mainLayout;
};

#endif //_THERMOMETER_WIDGET_H_
