#pragma once

#include <Wt/WDoubleSpinBox.h>
#include <Wt/WSpinBox.h>

#include "BaseEventEditor.hpp"
#include "Device.hpp"
#include "DeviceComboBox.hpp"
#include "Event.hpp"

class ThermostatEventEditor final : public BaseEventEditor {
public:
    ThermostatEventEditor();

    virtual ~ThermostatEventEditor() = default;

    virtual void SetDevices(const Devices& devices) override;

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
