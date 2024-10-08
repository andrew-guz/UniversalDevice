#include "EventReceiverWidget.hpp"

#include "Defines.hpp"
#include "EventWidgetHelper.hpp"
#include "Marshaling.hpp"
#include "RelayState.hpp"
#include "ThermometerLedBrightness.hpp"

using namespace Wt;

EventReceiverWidget::EventReceiverWidget() :
    WContainerWidget(),
    IEventEditorWidget() {
    _mainLayout = setLayout(std::make_unique<WGridLayout>());
    _mainLayout->setContentsMargins(0, 0, 0, 0);

    _mainLayout->addWidget(std::make_unique<WText>("Получатель события:"), 0, 0, 1, 2);
    _receivers = _mainLayout->addWidget(std::make_unique<DeviceComboBox>(), 1, 0, 1, 2);
    _receivers->changed().connect([this]() { OnReceiverChanged(); });
    _brightnessText = _mainLayout->addWidget(std::make_unique<WText>("Яркость:"), 2, 0);
    _brightness = _mainLayout->addWidget(std::make_unique<WSpinBox>(), 2, 1);
    _brightness->setMinimum(MIN_BRIGHTNESS);
    _brightness->setMaximum(MAX_BRIGHTNESS);
    _relayState = _mainLayout->addWidget(std::make_unique<WCheckBox>("Включить"), 3, 0, 1, 2);
}

void EventReceiverWidget::SetDevices(const std::vector<ExtendedComponentDescription>& devices) {
    _receivers->SetDevices(devices);
    OnReceiverChanged();
}

void EventReceiverWidget::Cleanup() {
    _receivers->setCurrentIndex(-1);
    _brightness->setValue(MIN_BRIGHTNESS);
    _relayState->setChecked(false);
    OnReceiverChanged();
}

void EventReceiverWidget::FillUi(const Event& event) {
    _receivers->SetSelectedDevice(event._receiver._id);
    if (event._receiver.isDeviceType() && event._receiver.getDeviceType() == DeviceType::Thermometer) {
        auto thermometerLedBrightness = event._command.get<ThermometerLedBrightness>();
        _brightness->setValue(thermometerLedBrightness._brightness);
    } else if (event._receiver.isDeviceType() &&
               (event._receiver.getDeviceType() == DeviceType::Relay || event._receiver.getDeviceType() == DeviceType::MotionRelay)) {
        auto relayState = event._command.get<RelayState>();
        _relayState->setChecked(relayState._state);
    }
}

bool EventReceiverWidget::IsValid() const { return _receivers->IsValid(); }

void EventReceiverWidget::FillFromUi(Event& event) const {
    event._receiver = _receivers->GetSelectedDevice();
    if (event._receiver.isDeviceType() && event._receiver.getDeviceType() == DeviceType::Thermometer) {
        ThermometerLedBrightness thermometerLedBrightness;
        thermometerLedBrightness._brightness = _brightness->value();
        event._command = thermometerLedBrightness;
    } else if (event._receiver.isDeviceType() &&
               (event._receiver.getDeviceType() == DeviceType::Relay || event._receiver.getDeviceType() == DeviceType::MotionRelay)) {
        RelayState relayState;
        relayState._state = _relayState->isChecked();
        event._command = relayState;
    }
}

void EventReceiverWidget::OnReceiverChanged() {
    if (!_receivers->IsValid())
        EventWidgetHelper::Hide(_brightnessText, _brightness, _relayState);
    else {
        const auto& selectedDevice = _receivers->GetSelectedDevice();
        if (selectedDevice.isDeviceType() && selectedDevice.getDeviceType() == DeviceType::Thermometer) {
            EventWidgetHelper::Hide(_relayState);
            EventWidgetHelper::Show(_brightnessText, _brightness);
        } else if (selectedDevice.isDeviceType() &&
                   (selectedDevice.getDeviceType() == DeviceType::Relay || selectedDevice.getDeviceType() == DeviceType::MotionRelay)) {
            EventWidgetHelper::Hide(_brightnessText, _brightness);
            EventWidgetHelper::Show(_relayState);
        }
    }
}
