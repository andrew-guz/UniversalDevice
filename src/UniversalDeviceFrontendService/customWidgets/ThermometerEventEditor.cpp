#include "ThermometerEventEditor.hpp"

#include <Wt/WGlobal.h>
#include <Wt/WHBoxLayout.h>

#include "ThermometerEvent.hpp"

using namespace Wt;

ThermometerEventEditor::ThermometerEventEditor() :
    BaseEventEditor() {
    _mainLayout->addWidget(std::make_unique<WText>("Генератор события:"), 0, AlignmentFlag::Top);
    _provider = _mainLayout->addWidget(std::make_unique<DeviceComboBox>(), 0, AlignmentFlag::Top);

    auto temperatureCanvas = _mainLayout->addWidget(std::make_unique<WContainerWidget>(), 0, AlignmentFlag::Top);
    auto temperatureLayout = temperatureCanvas->setLayout(std::make_unique<WHBoxLayout>());
    temperatureLayout->setContentsMargins(0, 0, 0, 0);

    temperatureLayout->addWidget(std::make_unique<WText>("Температура:"), 0, AlignmentFlag::Top);
    _temperature = temperatureLayout->addWidget(std::make_unique<WSpinBox>(), 1, AlignmentFlag::Top);
    _temperature->setMinimum(-40);
    _temperature->setMaximum(40);
    _lower = temperatureLayout->addWidget(std::make_unique<WCheckBox>("Ниже заданной"), 0, AlignmentFlag::Top);

    _receiver = _mainLayout->addWidget(std::make_unique<EventReceiverWidget>(), 0, AlignmentFlag::Top);
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
