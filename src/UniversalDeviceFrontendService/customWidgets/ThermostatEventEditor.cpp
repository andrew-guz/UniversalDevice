#include "ThermostatEventEditor.hpp"

#include <memory>

#include <Wt/WGlobal.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WValidator.h>

#include "BaseEventEditor.hpp"
#include "Device.hpp"
#include "DeviceComboBox.hpp"
#include "Enums.hpp"
#include "Event.hpp"
#include "Provider.hpp"
#include "Receiver.hpp"
#include "ThermostatEvent.hpp"
#include "Uuid.hpp"

using namespace Wt;

ThermostatEventEditor::ThermostatEventEditor() :
    BaseEventEditor() {
    _mainLayout->addWidget(std::make_unique<WText>("Генератор события:"), 0, AlignmentFlag::Top);
    _provider = _mainLayout->addWidget(std::make_unique<DeviceComboBox>(), 0, AlignmentFlag::Top);

    auto temperatureCanvas = _mainLayout->addWidget(std::make_unique<Wt::WContainerWidget>(), 0, Wt::AlignmentFlag::Top);
    auto temperatureLayout = temperatureCanvas->setLayout(std::make_unique<Wt::WHBoxLayout>());
    temperatureLayout->setContentsMargins(0, 0, 0, 0);

    temperatureLayout->addWidget(std::make_unique<WText>("Температура:"), 0, AlignmentFlag::Top);
    _temperature = temperatureLayout->addWidget(std::make_unique<WSpinBox>(), 1, AlignmentFlag::Top);
    _temperature->setMinimum(-40);
    _temperature->setMaximum(40);
    temperatureLayout->addWidget(std::make_unique<WText>("Дельта:"), 0, AlignmentFlag::Top);
    _delta = temperatureLayout->addWidget(std::make_unique<WDoubleSpinBox>(), 1, AlignmentFlag::Top);
    _delta->setMinimum(0.0f);
    _delta->setMaximum(5.0f);

    _mainLayout->addWidget(std::make_unique<WText>("Получатель события:"), 0, AlignmentFlag::Top);
    _receiver = _mainLayout->addWidget(std::make_unique<DeviceComboBox>(), 0, AlignmentFlag::Top);
}

void ThermostatEventEditor::SetDevices(const Devices& devices) {
    BaseEventEditor::SetDevices(devices);
    _provider->SetDevices(FilteredDevices(DeviceType::Thermometer));
    _receiver->SetDevices(FilteredDevices(DeviceType::Relay));
}

void ThermostatEventEditor::Cleanup() {
    BaseEventEditor::Cleanup();
    _provider->SetSelectedDevice(Uuid::Empty());
    _temperature->setValue(0);
    _delta->setValue(0.5f);
    _receiver->SetSelectedDevice(Uuid::Empty());
}

void ThermostatEventEditor::FillUi(const Event& event) {
    BaseEventEditor::FillUi(event);
    const ThermostatEvent& thermostatEvent = std::get<ThermostatEvent>(event);
    _provider->SetSelectedDevice(GetProviderId(thermostatEvent._provider));
    _temperature->setValue(thermostatEvent._temperature);
    _delta->setValue(thermostatEvent._delta);
    _receiver->SetSelectedDevice(thermostatEvent._receiver._id);
}

bool ThermostatEventEditor::IsValid() const {
    return BaseEventEditor::IsValid() && _provider->IsValid() && _temperature->validate() == ValidationState::Valid &&
           _delta->validate() == ValidationState::Valid && _receiver->IsValid();
}

void ThermostatEventEditor::FillFromUi(Event& event) const {
    BaseEventEditor::FillFromUi(event);
    ThermostatEvent& thermostatEvent = std::get<ThermostatEvent>(event);
    thermostatEvent._provider = _provider->GetSelectedDevice();
    thermostatEvent._temperature = _temperature->value();
    thermostatEvent._delta = _delta->value();
    thermostatEvent._receiver = _receiver->GetSelectedDevice();
}
