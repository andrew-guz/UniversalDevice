#ifndef _DEVICES_WIDGET_H_
#define _DEVICES_WIDGET_H_

#include <Wt/WContainerWidget.h>
#include <Wt/WGridLayout.h>

#include "BaseStackWidget.h"
#include "Settings.h"
#include "ExtendedComponentDescription.h"

class DeviceButton;

class DevicesWidget final : public Wt::WContainerWidget, public BaseStackWidget
{
public:
    DevicesWidget(IStackHolder* stackHolder, const Settings& settings);

    virtual ~DevicesWidget() = default;

    virtual void Initialize(const std::string& data) override;

private:
    void Clear();

    void Refresh();

    DeviceButton* AddButtonToLayout(Wt::WGridLayout* layout, const ExtendedComponentDescription& description, int& row, int& column);

private:
    Wt::WGridLayout*                        _mainLayout;
    std::map<Wt::WWidget*, Wt::WLayout*>    _deviceWidgets;
};

#endif //_DEVICES_WIDGET_H_
