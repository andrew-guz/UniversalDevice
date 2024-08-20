#include "TimerEventEditor.hpp"

#include "Constants.hpp"
#include "Enums.hpp"
#include "Marshaling.hpp"
#include "TimerEvent.hpp"
#include "Wt/WGlobal.h"

TimerEventEditor::TimerEventEditor() : BaseEventEditor() {
    _mainLayout->addWidget(std::make_unique<Wt::WText>("Час:"), 2, 0);
    _hour = _mainLayout->addWidget(std::make_unique<Wt::WSpinBox>(), 2, 1);
    _hour->setMinimum(0);
    _hour->setMaximum(23);
    _mainLayout->addWidget(std::make_unique<Wt::WText>("Минута:"), 3, 0);
    _minute = _mainLayout->addWidget(std::make_unique<Wt::WSpinBox>(), 3, 1);
    _minute->setMinimum(0);
    _minute->setMaximum(59);
    _receiver = _mainLayout->addWidget(std::make_unique<EventReceiverWidget>(), 4, 0, 1, 2);
}

void TimerEventEditor::SetDevices(const std::vector<ExtendedComponentDescription>& devices) {
    BaseEventEditor::SetDevices(devices);
    _receiver->SetDevices(devices);
}

void TimerEventEditor::Cleanup() {
    BaseEventEditor::Cleanup();
    _hour->setValue(0);
    _minute->setValue(0);
    _receiver->Cleanup();
}

void TimerEventEditor::FillUi(const Event& event) {
    BaseEventEditor::FillUi(event);
    const TimerEvent& timerEvent = dynamic_cast<const TimerEvent&>(event);
    _hour->setValue(timerEvent._hour);
    _minute->setValue(timerEvent._minute);
    _receiver->FillUi(event);
}

bool TimerEventEditor::IsValid() const {
    return BaseEventEditor::IsValid() && _hour->validate() == Wt::ValidationState::Valid && _minute->validate() == Wt::ValidationState::Valid &&
           _receiver->IsValid();
}

void TimerEventEditor::FillFromUi(Event& event) const {
    BaseEventEditor::FillFromUi(event);
    TimerEvent& timerEvent = dynamic_cast<TimerEvent&>(event);
    timerEvent._provider._id = Constants::PredefinedIdTimer;
    timerEvent._provider._type = EventTypeToString(EventType::Timer);
    timerEvent._hour = _hour->value();
    timerEvent._minute = _minute->value();
    _receiver->FillFromUi(event);
}
