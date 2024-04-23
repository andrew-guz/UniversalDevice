#pragma once

#include <Wt/WDoubleSpinBox.h>
#include <Wt/WSpinBox.h>

#include "BaseEventEditor.hpp"
#include "DeviceComboBox.hpp"
#include "EventReceiverWidget.hpp"

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
