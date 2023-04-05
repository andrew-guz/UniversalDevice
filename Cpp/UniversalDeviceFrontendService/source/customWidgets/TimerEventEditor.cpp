#include "TimerEventEditor.h"

#include "Constants.h"
#include "TimerEvent.h"

using namespace Wt;

TimerEventEditor::TimerEventEditor(int servicePort) :
    BaseEventEditor(servicePort)
{
    _mainLayout->addWidget(std::make_unique<WText>("Час:"), 2, 0);
    _hour = _mainLayout->addWidget(std::make_unique<WSpinBox>(), 2, 1);
    _hour->setMinimum(0);
    _hour->setMaximum(23);
    _mainLayout->addWidget(std::make_unique<WText>("Минута:"), 3, 0);
    _minute = _mainLayout->addWidget(std::make_unique<WSpinBox>(), 3, 1);
    _minute->setMinimum(0);
    _minute->setMaximum(59);
}

void TimerEventEditor::Cleanup()
{
    BaseEventEditor::Cleanup();
    _hour->setValue(0);
    _minute->setValue(0);
}

void TimerEventEditor::FillUi(const Event& event)
{
    BaseEventEditor::FillUi(event);
    const TimerEvent& timerEvent = dynamic_cast<const TimerEvent&>(event);
    _hour->setValue(timerEvent._hour);
    _minute->setValue(timerEvent._minute);
}

bool TimerEventEditor::IsValid() const
{
    return BaseEventEditor::IsValid() &&
        (_hour->validate() == ValidationState::Valid) &&
        (_minute->validate() == ValidationState::Valid);
}

void TimerEventEditor::FillFromUi(Event& event) const
{
    TimerEvent& timerEvent = dynamic_cast<TimerEvent&>(event);
    //TODO - id?
    BaseEventEditor::FillFromUi(event);
    timerEvent._provider._id = Constants::PredefinedIdTimer;
    timerEvent._provider._type = Constants::EventTypeTimer;
    timerEvent._hour = _hour->value();
    timerEvent._minute = _minute->value();
    //TODO receiver?    
}