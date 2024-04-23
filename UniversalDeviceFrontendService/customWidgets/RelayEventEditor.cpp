#include "RelayEventEditor.hpp"

#include "Constants.hpp"
#include "RelayEvent.hpp"

using namespace Wt;

RelayEventEditor::RelayEventEditor() : BaseEventEditor() {
    _mainLayout->addWidget(std::make_unique<WText>("Генератор события:"), 2, 0, 1, 2);
    _provider = _mainLayout->addWidget(std::make_unique<DeviceComboBox>(), 3, 0, 1, 2);
    _state = _mainLayout->addWidget(std::make_unique<WCheckBox>("Включено"), 5, 0, 1, 2);
    _receiver = _mainLayout->addWidget(std::make_unique<EventReceiverWidget>(), 6, 0, 1, 2);
}

void RelayEventEditor::SetDevices(const std::vector<ExtendedComponentDescription>& devices) {
    BaseEventEditor::SetDevices(devices);
    _provider->SetDevices(FilteredDevices({Constants::DeviceTypeRelay, Constants::DeviceTypeMotionRelay}));
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
    const RelayEvent& relayEvent = dynamic_cast<const RelayEvent&>(event);
    _provider->SetSelectedDevice(event._provider._id);
    _state->setChecked(relayEvent._state);
    _receiver->FillUi(event);
}

bool RelayEventEditor::IsValid() const { return BaseEventEditor::IsValid() && _provider->IsValid() && _receiver->IsValid(); }

void RelayEventEditor::FillFromUi(Event& event) const {
    BaseEventEditor::FillFromUi(event);
    RelayEvent& relayEvent = dynamic_cast<RelayEvent&>(event);
    event._provider = _provider->GetSelectedDevice();
    relayEvent._state = _state->isChecked();
    _receiver->FillFromUi(event);
}
