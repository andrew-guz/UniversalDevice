#pragma once

#include <Wt/WSpinBox.h>

#include "BaseEventEditor.hpp"
#include "Device.hpp"
#include "Enums.hpp"
#include "Event.hpp"
#include "EventReceiverWidget.hpp"

class SunriseSunsetEditor final : public BaseEventEditor {
public:
    SunriseSunsetEditor(EventType type);

    virtual ~SunriseSunsetEditor() = default;

    virtual void SetDevices(const Devices& devices) override;

    virtual void Cleanup() override;

    virtual void FillUi(const Event& event) override;

    virtual bool IsValid() const override;

    virtual void FillFromUi(Event& event) const override;

private:
    EventType _type;
    EventReceiverWidget* _receiver;
};
