#ifndef _DEVICES_WIDGET_H_
#define _DEVICES_WIDGET_H_

#include <Wt/WContainerWidget.h>
#include <Wt/WGridLayout.h>
#include <Wt/WPushButton.h>

#include "BaseStackWidget.h"
#include "Settings.h"

class DevicesWidget final : public Wt::WContainerWidget, public BaseStackWidget
{
public:
    DevicesWidget(IStackHolder* stackHolder, const Settings& settings);

    virtual ~DevicesWidget() = default;

    virtual void Initialize(const std::string& data) override;

private:
    void Clear();

    void Refresh();

private:
    Wt::WGridLayout*                _mainLayout;
    std::vector<Wt::WPushButton*>   _deviceButtons;
};

#endif //_DEVICES_WIDGET_H_
