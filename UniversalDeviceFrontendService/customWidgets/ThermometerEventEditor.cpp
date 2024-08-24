#include "ThermometerEventEditor.hpp"

#include "Constants.hpp"
#include "ThermometerEvent.hpp"

using namespace Wt;

ThermometerEventEditor::ThermometerEventEditor() : BaseEventEditor() {
    _mainLayout->addWidget(std::make_unique<WText>("Генератор события:"), 2, 0, 1, 2);
    _provider = _mainLayout->addWidget(std::make_unique<DeviceComboBox>(), 3, 0, 1, 2);
    _mainLayout->addWidget(std::make_unique<WText>("Температура:"), 4, 0);
    _temperature = _mainLayout->addWidget(std::make_unique<WSpinBox>(), 4, 1);
    _temperature->setMinimum(-40);
    _temperature->setMaximum(40);
    _lower = _mainLayout->addWidget(std::make_unique<WCheckBox>("Ниже заданной"), 5, 0, 1, 2);
    _receiver = _mainLayout->addWidget(std::make_unique<EventReceiverWidget>(), 6, 0, 1, 2);
}

void ThermometerEventEditor::SetDevices(const std::vector<ExtendedComponentDescription>& devices) {
    BaseEventEditor::SetDevices(devices);
    _provider->SetDevices(FilteredDevices(DeviceType::Thermometer));
    _receiver->SetDevices(devices);
}

void ThermometerEventEditor::Cleanup() {
    BaseEventEditor::Cleanup();
    _provider->SetSelectedDevice(Uuid::Empty());
    _temperature->setValue(0);
    _lower->setChecked(false);
    _receiver->Cleanup();
}

void ThermometerEventEditor::FillUi(const Event& event) {
    BaseEventEditor::FillUi(event);
    const ThermometerEvent& thermometerEvent = dynamic_cast<const ThermometerEvent&>(event);
    _provider->SetSelectedDevice(event._provider._id);
    _temperature->setValue(thermometerEvent._temperature);
    _lower->setChecked(thermometerEvent._lower);
    _receiver->FillUi(event);
}

bool ThermometerEventEditor::IsValid() const {
    return BaseEventEditor::IsValid() && _provider->IsValid() && _temperature->validate() == ValidationState::Valid && _receiver->IsValid();
}

void ThermometerEventEditor::FillFromUi(Event& event) const {
    BaseEventEditor::FillFromUi(event);
    ThermometerEvent& thermometerEvent = dynamic_cast<ThermometerEvent&>(event);
    event._provider = _provider->GetSelectedDevice();
    thermometerEvent._temperature = _temperature->value();
    thermometerEvent._lower = _lower->isChecked();
    _receiver->FillFromUi(event);
}
