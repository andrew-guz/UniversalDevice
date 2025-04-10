#include "SunriseSunsetEditor.hpp"

#include <cassert>

#include <Wt/WGlobal.h>
#include <Wt/WHBoxLayout.h>

#include "Enums.hpp"
#include "Marshaling.hpp"

SunriseSunsetEditor::SunriseSunsetEditor(EventType type) :
    BaseEventEditor(),
    _type(type) {
    assert(_type == EventType::Sunrise || _type == EventType::Sunset);

    _receiver = _mainLayout->addWidget(std::make_unique<EventReceiverWidget>(), 0, Wt::AlignmentFlag::Top);
}

void SunriseSunsetEditor::SetDevices(const std::vector<ExtendedComponentDescription>& devices) {
    BaseEventEditor::SetDevices(devices);
    _receiver->SetDevices(devices);
}

void SunriseSunsetEditor::Cleanup() {
    BaseEventEditor::Cleanup();
    _receiver->Cleanup();
}

void SunriseSunsetEditor::FillUi(const Event& event) {
    BaseEventEditor::FillUi(event);
    _receiver->FillUi(event);
}

bool SunriseSunsetEditor::IsValid() const { return BaseEventEditor::IsValid() && _receiver->IsValid(); }

void SunriseSunsetEditor::FillFromUi(Event& event) const {
    BaseEventEditor::FillFromUi(event);
    if (_type == EventType::Sunrise)
        event._provider._id = Constants::PredefinedIdSunrise;
    if (_type == EventType::Sunset)
        event._provider._id = Constants::PredefinedIdSunset;
    event._provider._type = _type;
    _receiver->FillFromUi(event);
}
