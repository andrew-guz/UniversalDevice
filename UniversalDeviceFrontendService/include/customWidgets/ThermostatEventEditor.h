#ifndef _THERMOSTAT_EVENT_EDITOR_H_
#define _THERMOSTAT_EVENT_EDITOR_H_

#include <Wt/WDoubleSpinBox.h>
#include <Wt/WSpinBox.h>

#include "BaseEventEditor.h"
#include "DeviceComboBox.h"
#include "EventReceiverWidget.h"

class ThermostatEventEditor final : public BaseEventEditor {
public:
    ThermostatEventEditor();

    virtual ~ThermostatEventEditor() = default;

    virtual void SetDevices(const std::vector<ExtendedComponentDescription>& devices) override;

    virtual void Cleanup() override;

    virtual void FillUi(const Event& event) override;

    virtual bool IsValid() const override;

    virtual void FillFromUi(Event& event) const override;

private:
    DeviceComboBox* _provider;
    Wt::WSpinBox* _temperature;
    Wt::WDoubleSpinBox* _delta;
    DeviceComboBox* _receiver;
};

#endif //_THERMOSTAT_EVENT_EDITOR_H_
