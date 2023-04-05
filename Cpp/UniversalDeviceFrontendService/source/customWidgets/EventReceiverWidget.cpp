#include "EventReceiverWidget.h"

#include "Defines.h"
#include "EventWidgetHelper.h"

using namespace Wt;

EventReceiverWidget::EventReceiverWidget(const std::vector<ComponentDescription>& devices) :
    WContainerWidget(),
    IEventEditorWidget()    
{
    _mainLayout = setLayout(std::make_unique<WGridLayout>());
    
    _mainLayout->addWidget(std::make_unique<WText>("Получатель события:"), 0, 0, 1, 2);
    _receivers = _mainLayout->addWidget(std::make_unique<DeviceComboBox>(devices), 1, 0, 1, 2);
    _receivers->changed().connect([&]()
    {
        if (!_receivers->IsDeviceSelected())
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
    });
    _brightnessText = _mainLayout->addWidget(std::make_unique<WText>("Яркость:"), 2, 0);
    _brightness = _mainLayout->addWidget(std::make_unique<WSpinBox>(), 2, 1);
    _brightness->setMinimum(MIN_BRIGHTNESS);
    _brightness->setMaximum(MAX_BRIGHTNESS);
    _relayState = _mainLayout->addWidget(std::make_unique<WCheckBox>("Включить"), 3, 0, 1, 2);
}

void EventReceiverWidget::Cleanup()
{
    _receivers->setCurrentIndex(-1);
    _brightness->setValue(MIN_BRIGHTNESS);
    _relayState->setChecked(false);
}

void EventReceiverWidget::FillUi(const Event& event)
{
    _receivers->SetSelectedDevice(event._receiver._id);
    //parse command to gent what is going on
}

bool EventReceiverWidget::IsValid() const
{
    return _receivers->IsDeviceSelected();
}

void EventReceiverWidget::FillFromUi(Event& event) const
{
    event._receiver = _receivers->GetSelectedDevice();
    //create command
}
