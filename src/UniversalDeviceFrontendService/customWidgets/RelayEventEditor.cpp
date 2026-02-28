#include "RelayEventEditor.hpp"

#include <memory>

#include <Wt/WGlobal.h>

#include "BaseEventEditor.hpp"
#include "Device.hpp"
#include "DeviceComboBox.hpp"
#include "Enums.hpp"
#include "Event.hpp"
#include "EventReceiverWidget.hpp"
#include "Provider.hpp"
#include "RelayEvent.hpp"
#include "Uuid.hpp"

using namespace Wt;

RelayEventEditor::RelayEventEditor() :
    BaseEventEditor() {

    _mainLayout->addWidget(std::make_unique<WText>("Генератор события:"), 0, AlignmentFlag::Top);
    _provider = _mainLayout->addWidget(std::make_unique<DeviceComboBox>(), 0, AlignmentFlag::Top);
    _state = _mainLayout->addWidget(std::make_unique<WCheckBox>("Включено"), 0, AlignmentFlag::Top);
    _receiver = _mainLayout->addWidget(std::make_unique<EventReceiverWidget>(), 0, AlignmentFlag::Top);
}

void RelayEventEditor::SetDevices(const Devices& devices) {
    BaseEventEditor::SetDevices(devices);
    _provider->SetDevices(FilteredDevices({ DeviceType::Relay, DeviceType::MotionRelay }));
    _receiver->SetDevices(devices);
}

void RelayEventEditor::Cleanup() {
    BaseEventEditor::Cleanup();
    _provider->SetSelectedDevice(Uuid::Empty());
    _state->setChecked(false);
    _receiver->Cleanup();
}

void RelayEventEditor::FillUi(const Event& event) {
    BaseEventEditor::FillUi(event);
    const RelayEvent& relayEvent = std::get<RelayEvent>(event);
    _provider->SetSelectedDevice(GetProviderId(relayEvent._provider));
    _state->setChecked(relayEvent._state);
    _receiver->FillUi(event);
}

bool RelayEventEditor::IsValid() const { return BaseEventEditor::IsValid() && _provider->IsValid() && _receiver->IsValid(); }

void RelayEventEditor::FillFromUi(Event& event) const {
    BaseEventEditor::FillFromUi(event);
    RelayEvent& relayEvent = std::get<RelayEvent>(event);
    relayEvent._provider = _provider->GetSelectedDevice();
    relayEvent._state = _state->isChecked();
    _receiver->FillFromUi(event);
}
