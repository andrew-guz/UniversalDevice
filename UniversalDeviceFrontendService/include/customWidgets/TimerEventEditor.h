#ifndef _TIMER_EVENT_EDITOR_H_
#define _TIMER_EVENT_EDITOR_H_

#include <Wt/WSpinBox.h>

#include "BaseEventEditor.h"
#include "EventReceiverWidget.h"

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

#endif //_TIMER_EVENT_EDITOR_H_
