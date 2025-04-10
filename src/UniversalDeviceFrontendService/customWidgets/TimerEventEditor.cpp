#include "TimerEventEditor.hpp"

#include <Wt/WGlobal.h>
#include <Wt/WHBoxLayout.h>

#include "Constants.hpp"
#include "Enums.hpp"
#include "Marshaling.hpp"
#include "TimerEvent.hpp"

TimerEventEditor::TimerEventEditor() :
    BaseEventEditor() {
    auto timeCanvas = _mainLayout->addWidget(std::make_unique<Wt::WContainerWidget>(), 0, Wt::AlignmentFlag::Top);
    auto timeLayout = timeCanvas->setLayout(std::make_unique<Wt::WHBoxLayout>());
    timeLayout->setContentsMargins(0, 0, 0, 0);

    timeLayout->addWidget(std::make_unique<Wt::WText>("Час:"), 0, Wt::AlignmentFlag::Top);
    _hour = timeLayout->addWidget(std::make_unique<Wt::WSpinBox>(), 1, Wt::AlignmentFlag::Top);
    _hour->setMinimum(0);
    _hour->setMaximum(23);

    timeLayout->addWidget(std::make_unique<Wt::WText>("Минута:"), 0, Wt::AlignmentFlag::Top);
    _minute = timeLayout->addWidget(std::make_unique<Wt::WSpinBox>(), 1, Wt::AlignmentFlag::Top);
    _minute->setMinimum(0);
    _minute->setMaximum(59);

    _receiver = _mainLayout->addWidget(std::make_unique<EventReceiverWidget>(), 0, Wt::AlignmentFlag::Top);
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
    timerEvent._provider._type = EventType::Timer;
    timerEvent._hour = _hour->value();
    timerEvent._minute = _minute->value();
    _receiver->FillFromUi(event);
}
