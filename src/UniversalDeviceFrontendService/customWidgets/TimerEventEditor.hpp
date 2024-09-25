#pragma once

#include <Wt/WSpinBox.h>

#include "BaseEventEditor.hpp"
#include "EventReceiverWidget.hpp"

class TimerEventEditor final : public BaseEventEditor {
public:
    TimerEventEditor();

    virtual ~TimerEventEditor() = default;

    virtual void SetDevices(const std::vector<ExtendedComponentDescription>& devices) override;

    virtual void Cleanup() override;

    virtual void FillUi(const Event& event) override;

    virtual bool IsValid() const override;

    virtual void FillFromUi(Event& event) const override;

private:
    Wt::WSpinBox* _hour;
    Wt::WSpinBox* _minute;
    EventReceiverWidget* _receiver;
};
