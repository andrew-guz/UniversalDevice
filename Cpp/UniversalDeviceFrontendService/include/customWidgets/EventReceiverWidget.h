#ifndef _EVENT_RECEIVER_WIDGET_H_
#define _EVENT_RECEIVER_WIDGET_H_

#include <Wt/WContainerWidget.h>
#include <Wt/WGridLayout.h>
#include <Wt/WText.h>
#include <Wt/WSpinBox.h>
#include <Wt/WCheckBox.h>

#include "IEventEditorWidget.h"
#include "DeviceComboBox.h"

class EventReceiverWidget final : public Wt::WContainerWidget, public IEventEditorWidget
{
public:
    EventReceiverWidget();

    virtual ~EventReceiverWidget() = default;

    virtual void SetDevices(const std::vector<ExtendedComponentDescription>& devices) override;

    virtual void Cleanup() override;

    virtual void FillUi(const Event& event) override;

    virtual bool IsValid() const override;

    virtual void FillFromUi(Event& event) const override;

private:
    void OnReceiverChanged();

private:
    Wt::WGridLayout*    _mainLayout;
    DeviceComboBox*     _receivers;
    Wt::WText*          _brightnessText;
    Wt::WSpinBox*       _brightness;
    Wt::WCheckBox*      _relayState;
};

#endif //_EVENT_RECEIVER_WIDGET_H_
