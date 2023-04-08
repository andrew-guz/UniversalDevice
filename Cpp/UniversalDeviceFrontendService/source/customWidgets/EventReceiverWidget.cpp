#include "EventReceiverWidget.h"

#include "Defines.h"
#include "JsonExtension.h"
#include "EventWidgetHelper.h"
#include "ThermometerLedBrightness.h"
#include "RelayState.h"

using namespace Wt;

EventReceiverWidget::EventReceiverWidget() :
    WContainerWidget(),
    IEventEditorWidget()    
{
    _mainLayout = setLayout(std::make_unique<WGridLayout>());
    _mainLayout->setContentsMargins(0, 0, 0, 0);
    
    _mainLayout->addWidget(std::make_unique<WText>("Получатель события:"), 0, 0, 1, 2);
    _receivers = _mainLayout->addWidget(std::make_unique<DeviceComboBox>(), 1, 0, 1, 2);
    _receivers->changed().connect([&]()
    {
        OnReceiverChanged();
    });
    _brightnessText = _mainLayout->addWidget(std::make_unique<WText>("Яркость:"), 2, 0);
    _brightness = _mainLayout->addWidget(std::make_unique<WSpinBox>(), 2, 1);
    _brightness->setMinimum(MIN_BRIGHTNESS);
    _brightness->setMaximum(MAX_BRIGHTNESS);
    _relayState = _mainLayout->addWidget(std::make_unique<WCheckBox>("Включить"), 3, 0, 1, 2);
}

void EventReceiverWidget::SetDevices(const std::vector<ExtendedComponentDescription>& devices)
{
    _receivers->SetDevices(devices);
    OnReceiverChanged();
}

void EventReceiverWidget::Cleanup()
{
    _receivers->setCurrentIndex(-1);
    _brightness->setValue(MIN_BRIGHTNESS);
    _relayState->setChecked(false);
    OnReceiverChanged();
}

void EventReceiverWidget::FillUi(const Event& event)
{
    _receivers->SetSelectedDevice(event._receiver._id);
    if (event._receiver._type == Constants::DeviceTypeThermometer)
    {
        auto thermometerLedBrightness = JsonExtension::CreateFromJson<ThermometerLedBrightness>(event._command);
        _brightness->setValue(thermometerLedBrightness._brightness);
    }
    else if (event._receiver._type == Constants::DeviceTypeRelay ||
        event._receiver._type == Constants::DeviceTypeMotionRelay)
    {
        auto relayState = JsonExtension::CreateFromJson<RelayState>(event._command);
        _relayState->setChecked(relayState._state);
    }
}

bool EventReceiverWidget::IsValid() const
{
    return _receivers->IsValid();
}

void EventReceiverWidget::FillFromUi(Event& event) const
{
    event._receiver = _receivers->GetSelectedDevice();
    if (event._receiver._type == Constants::DeviceTypeThermometer)
    {
        ThermometerLedBrightness thermometerLedBrightness;
        thermometerLedBrightness._brightness = _brightness->value();
        event._command = thermometerLedBrightness.ToJson();
    }
    else if (event._receiver._type == Constants::DeviceTypeRelay ||
        event._receiver._type == Constants::DeviceTypeMotionRelay)
    {
        RelayState relayState;
        relayState._state = _relayState->isChecked();
        event._command = relayState.ToJson();
    }
}

void EventReceiverWidget::OnReceiverChanged()
{
    if (!_receivers->IsValid())
        EventWidgetHelper::Hide(_brightnessText, _brightness, _relayState);
    else
    {
        if (_receivers->GetSelectedDevice()._type == Constants::DeviceTypeThermometer)
        {
            EventWidgetHelper::Hide(_relayState);
            EventWidgetHelper::Show(_brightnessText, _brightness);
        }
        else if (_receivers->GetSelectedDevice()._type == Constants::DeviceTypeRelay ||
            _receivers->GetSelectedDevice()._type == Constants::DeviceTypeMotionRelay)
        {
            EventWidgetHelper::Hide(_brightnessText, _brightness);
            EventWidgetHelper::Show(_relayState);
        }
    }
}
