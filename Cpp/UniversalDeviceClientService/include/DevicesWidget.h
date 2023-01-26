#ifndef _DEVICES_WIDGET_H_
#define _DEVICES_WIDGET_H_

#include <Wt/WContainerWidget.h>
#include <Wt/WGridLayout.h>
#include <Wt/WPushButton.h>

class DevicesWidget final : public Wt::WContainerWidget
{
public:
    DevicesWidget();

    virtual ~DevicesWidget() = default;

private:
    void Refresh();

private:
    Wt::WGridLayout* _mainLayout;
    Wt::WPushButton* _refreshButton;
};

#endif //_DEVICES_WIDGET_H_
