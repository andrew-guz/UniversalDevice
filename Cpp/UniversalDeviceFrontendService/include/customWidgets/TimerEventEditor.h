#ifndef _TIMER_EVENT_EDITOR_H_
#define _TIMER_EVENT_EDITOR_H_

#include <Wt/WSpinBox.h>

#include "BaseEventEditor.h"

class TimerEventEditor : public BaseEventEditor
{
public:
    TimerEventEditor(int servicePort);

    virtual ~TimerEventEditor() = default;

    virtual void Cleanup() override;

    virtual void FillUi(const Event& event) override;

    virtual bool IsValid() const override;

    virtual void FillFromUi(Event& event) const override;

private:
    Wt::WSpinBox* _hour;
    Wt::WSpinBox* _minute;
};

#endif //_TIMER_EVENT_EDITOR_H_
