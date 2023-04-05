#ifndef _RELAY_EVENT_EDITOR_H_
#define _RELAY_EVENT_EDITOR_H_

#include <Wt/WCheckBox.h>

#include "BaseEventEditor.h"
#include "DeviceComboBox.h"
#include "EventReceiverWidget.h"

class RelayEventEditor : public BaseEventEditor
{
public:
    RelayEventEditor(const std::vector<ComponentDescription>& devices);

    virtual ~RelayEventEditor() = default;

    virtual void Cleanup() override;

    virtual void FillUi(const Event& event) override;

    virtual bool IsValid() const override;

    virtual void FillFromUi(Event& event) const override;

private:
    DeviceComboBox*         _provider;
    Wt::WCheckBox*          _state;
    EventReceiverWidget*    _receiver;
};

#endif //_RELAY_EVENT_EDITOR_H_
