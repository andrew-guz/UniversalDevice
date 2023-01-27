#ifndef _DEVICES_WIDGET_H_
#define _DEVICES_WIDGET_H_

#include <Wt/WContainerWidget.h>
#include <Wt/WGridLayout.h>
#include <Wt/WPushButton.h>

#include "Settings.h"

class DevicesWidget final : public Wt::WContainerWidget
{
public:
    DevicesWidget(const Settings& settings);

    virtual ~DevicesWidget() = default;

private:
    void Clear();

    void Refresh();

private:
    const Settings&                 _settings;
    Wt::WGridLayout*                _mainLayout;
    Wt::WPushButton*                _refreshButton;
    std::vector<Wt::WPushButton*>   _deviceButtons;
};

#endif //_DEVICES_WIDGET_H_
