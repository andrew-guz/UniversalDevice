#include "EventReceiverWidget.hpp"

#include <memory>
#include <variant>

#include <Wt/WGlobal.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>
#include <boost/hof/match.hpp>

#include "Defines.hpp"
#include "Device.hpp"
#include "DeviceComboBox.hpp"
#include "Enums.hpp"
#include "Event.hpp"
#include "EventWidgetHelper.hpp"
#include "IEventEditorWidget.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "RelayState.hpp"
#include "ThermometerLedBrightness.hpp"

using namespace Wt;

EventReceiverWidget::EventReceiverWidget() :
    WContainerWidget(),
    IEventEditorWidget() {
    _mainLayout = setLayout(std::make_unique<WVBoxLayout>());
    _mainLayout->setContentsMargins(0, 0, 0, 0);

    auto receiverCanvas = _mainLayout->addWidget(std::make_unique<WContainerWidget>(), 0, AlignmentFlag::Top);
    auto receiverLayout = receiverCanvas->setLayout(std::make_unique<WHBoxLayout>());
    receiverLayout->setContentsMargins(0, 0, 0, 0);

    receiverLayout->addWidget(std::make_unique<WText>("Получатель события:"), 0, AlignmentFlag::Top | AlignmentFlag::Left);
    _receivers = receiverLayout->addWidget(std::make_unique<DeviceComboBox>(), 1, AlignmentFlag::Top);
    _receivers->changed().connect([this]() { OnReceiverChanged(); });

    auto brightnessCanvas = _mainLayout->addWidget(std::make_unique<WContainerWidget>(), 0, AlignmentFlag::Top);
    auto brightnessLayout = brightnessCanvas->setLayout(std::make_unique<WHBoxLayout>());
    brightnessLayout->setContentsMargins(0, 0, 0, 0);

    _brightnessText = brightnessLayout->addWidget(std::make_unique<WText>("Яркость:"), 0, AlignmentFlag::Top | AlignmentFlag::Left);
    _brightness = brightnessLayout->addWidget(std::make_unique<WSpinBox>(), 1, AlignmentFlag::Top);
    _brightness->setMinimum(MIN_BRIGHTNESS);
    _brightness->setMaximum(MAX_BRIGHTNESS);

    _relayState = _mainLayout->addWidget(std::make_unique<WCheckBox>("Включить"), 0, AlignmentFlag::Top);
}

void EventReceiverWidget::SetDevices(const Devices& devices) {
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
    const auto eventFunction = [&](const auto& event) {
        _receivers->SetSelectedDevice(event._receiver._id);
        std::visit(boost::hof::match([&](const ThermometerLedBrightness& command) { _brightness->setValue(command._brightness); },
                                     [&](const RelayState& command) { _relayState->setChecked(command._state); }),
                   event._command);
    };
    std::visit(boost::hof::match([&](const TimerEvent& timerEvent) { eventFunction(timerEvent); },
                                 [&](const ThermometerEvent& thermometerEvent) { eventFunction(thermometerEvent); },
                                 [&](const RelayEvent& relayEvent) { eventFunction(relayEvent); },
                                 [&](const ThermostatEvent& thermostatEvent) { LOG_ERROR_MSG("No command for ThermostatEvent"); },
                                 [&](const SunriseEvent& sunriseEvent) { eventFunction(sunriseEvent); },
                                 [&](const SunsetEvent& sunsetEvent) { eventFunction(sunsetEvent); }),
               event);
    OnReceiverChanged();
}

bool EventReceiverWidget::IsValid() const { return _receivers->IsValid(); }

void EventReceiverWidget::FillFromUi(Event& event) const {
    const auto eventFunction = [&](auto& event) {
        event._receiver = _receivers->GetSelectedDevice();
        std::visit(boost::hof::match([&](ThermometerLedBrightness& command) { command._brightness = _brightness->value(); },
                                     [&](RelayState& command) { command._state = _relayState->isChecked(); }),
                   event._command);
    };
    std::visit(boost::hof::match([&](TimerEvent& timerEvent) { eventFunction(timerEvent); },
                                 [&](ThermometerEvent& thermometerEvent) { eventFunction(thermometerEvent); },
                                 [&](RelayEvent& relayEvent) { eventFunction(relayEvent); },
                                 [&](ThermostatEvent& thermostatEvent) { LOG_ERROR_MSG("No command for ThermostatEvent"); },
                                 [&](SunriseEvent& sunriseEvent) { eventFunction(sunriseEvent); },
                                 [&](SunsetEvent& sunsetEvent) { eventFunction(sunsetEvent); }),
               event);
}

void EventReceiverWidget::OnReceiverChanged() {
    if (!_receivers->IsValid())
        EventWidgetHelper::Hide(_brightnessText, _brightness, _relayState);
    else {
        const auto& selectedDevice = _receivers->GetSelectedDevice();
        if (selectedDevice._type == DeviceType::Thermometer) {
            EventWidgetHelper::Hide(_relayState);
            EventWidgetHelper::Show(_brightnessText, _brightness);
        } else if (selectedDevice._type == DeviceType::Relay || selectedDevice._type == DeviceType::MotionRelay) {
            EventWidgetHelper::Hide(_brightnessText, _brightness);
            EventWidgetHelper::Show(_relayState);
        }
    }
}
