#include "SunriseSunsetEditor.hpp"

#include <cassert>
#include <memory>
#include <variant>

#include <Wt/WGlobal.h>
#include <Wt/WHBoxLayout.h>
#include <boost/hof/match.hpp>

#include "BaseEventEditor.hpp"
#include "Constants.hpp"
#include "Device.hpp"
#include "Enums.hpp"
#include "Event.hpp"
#include "EventReceiverWidget.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "Provider.hpp"
#include "SunriseEvent.hpp"
#include "SunsetEvent.hpp"

SunriseSunsetEditor::SunriseSunsetEditor(EventType type) :
    BaseEventEditor(),
    _type(type) {
    assert(_type == EventType::Sunrise || _type == EventType::Sunset);

    _receiver = _mainLayout->addWidget(std::make_unique<EventReceiverWidget>(), 0, Wt::AlignmentFlag::Top);
}

void SunriseSunsetEditor::SetDevices(const Devices& devices) {
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
    std::visit(boost::hof::match(
                   [&](SunriseEvent& e) {
                       e._provider.emplace<EventProvider>(EventProvider{
                           ._id = Constants::PredefinedIdSunrise,
                       });
                   },
                   [&](SunsetEvent& e) {
                       e._provider.emplace<EventProvider>(EventProvider{
                           ._id = Constants::PredefinedIdSunset,
                       });
                   },
                   [&](auto& e) { LOG_ERROR_MSG("Invalid event type in SunriseSunsetEditor"); }),
               event);
    _receiver->FillFromUi(event);
}
