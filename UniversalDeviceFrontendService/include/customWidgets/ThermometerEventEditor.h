#ifndef _THERMOMETER_EVENT_EDITOR_H_
#define _THERMOMETER_EVENT_EDITOR_H_

#include <Wt/WCheckBox.h>
#include <Wt/WSpinBox.h>

#include "BaseEventEditor.h"
#include "DeviceComboBox.h"
#include "EventReceiverWidget.h"

class ThermometerEventEditor final : public BaseEventEditor {
public:
    ThermometerEventEditor();

    virtual ~ThermometerEventEditor() = default;

    virtual void SetDevices(const std::vector<ExtendedComponentDescription>& devices) override;

    virtual void Cleanup() override;

    virtual void FillUi(const Event& event) override;

    virtual bool IsValid() const override;

    virtual void FillFromUi(Event& event) const override;

private:
    DeviceComboBox* _provider;
    Wt::WSpinBox* _temperature;
    Wt::WCheckBox* _lower;
    EventReceiverWidget* _receiver;
};

#endif //_THERMOMETER_EVENT_EDITOR_H_
