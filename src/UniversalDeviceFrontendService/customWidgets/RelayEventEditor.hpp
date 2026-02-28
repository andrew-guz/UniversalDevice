#pragma once

#include <Wt/WCheckBox.h>

#include "BaseEventEditor.hpp"
#include "Device.hpp"
#include "DeviceComboBox.hpp"
#include "Event.hpp"
#include "EventReceiverWidget.hpp"

class RelayEventEditor final : public BaseEventEditor {
public:
    RelayEventEditor();

    virtual ~RelayEventEditor() = default;

    virtual void SetDevices(const Devices& devices) override;

    virtual void Cleanup() override;

    virtual void FillUi(const Event& event) override;

    virtual bool IsValid() const override;

    virtual void FillFromUi(Event& event) const override;

private:
    DeviceComboBox* _provider;
    Wt::WCheckBox* _state;
    EventReceiverWidget* _receiver;
};
