#include "MotionRelayWidget.h"

#include <Wt/WTimer.h>
#include <Wt/WEvent.h>

#include "Defines.h"
#include "Constants.h"
#include "Logger.h"
#include "ComponentDescription.h"
#include "ExtendedComponentDescription.h"
#include "DeviceName.h"
#include "WidgetHelper.h"
#include "RelayState.h"

using namespace Wt;

MotionRelayWidget::MotionRelayWidget(IStackHolder* stackHolder, const Settings& settings) :
    BaseDeviceWidget(stackHolder, settings)
{
    _motionText = _mainLayout->addWidget(std::make_unique<WText>(), 3, 1, AlignmentFlag::Center);
    _motionText->setText(WidgetHelper::TextWithFontSize("Нет движения", 80));

    _stateText = _mainLayout->addWidget(std::make_unique<WText>(), 4, 1, AlignmentFlag::Center);
    _stateText->setText(WidgetHelper::TextWithFontSize("Выключено", 80));

    _stateButton = _mainLayout->addWidget(std::make_unique<WPushButton>(), 5, 1, AlignmentFlag::Center);
    _stateButton->setTextFormat(TextFormat::XHTML);
    _stateButton->setText(WidgetHelper::TextWithFontSize("Включить", 32));    
    _stateButton->setMinimumSize(200, 200);
    _stateButton->setMaximumSize(200, 200);
    _stateButton->clicked().connect([&](){
        OnStateButton();
    });
}

void MotionRelayWidget::Initialize()
{
    auto motionStateValues = GetValues<ExtendedMotionRelayCurrentState>(Constants::DeviceTypeMotionRelay);
    if (motionStateValues.size())
    {
        auto& motionStateValue = motionStateValues[0];
        _motionDetected = motionStateValue._motion;
        _deviceState = motionStateValue._state;
        _motionText->setText(WidgetHelper::TextWithFontSize(_motionDetected ? "Движение" : "Нет движения", 80));
        _stateText->setText(WidgetHelper::TextWithFontSize(_deviceState ? "Включено" : "Выключено", 80));
        _stateButton->setText(WidgetHelper::TextWithFontSize(_deviceState ? "Выключить" : "Включить", 32));
        auto timestamp = motionStateValue._timestamp;
        _timeText->setText(WidgetHelper::TextWithFontSize(TimeHelper::TimeToString(timestamp), 20));
    }
    else
        Clear(BaseDeviceWidget::ClearType::Data);
    _stateButton->setEnabled(true);
}

void MotionRelayWidget::ClearData()
{
    _timeText->setText(WidgetHelper::TextWithFontSize("", 20));
    _motionText->setText(WidgetHelper::TextWithFontSize("Нет движения", 80));
    _stateText->setText(WidgetHelper::TextWithFontSize("Выключено", 80));
    _stateButton->setText(WidgetHelper::TextWithFontSize("Включить", 32));
}

void MotionRelayWidget::OnSettingsButton()
{
    if (_deviceId.isEmpty())
        return;
    auto settings = GetSettings<MotionRelaySettings>();
    auto [dialog, layout, nameEdit, periodEdit, ok] = WidgetHelper::CreateNamePeriodSettingsDialog(this, 150, _deviceName, settings._period, true);
    //activityDelay
    layout->addWidget(std::make_unique<WText>("Задержка (мин):"), 2, 0);
    auto activityDelayEdit = layout->addWidget(std::make_unique<WSpinBox>(), 2, 1);
    activityDelayEdit->setMinimum(MIN_ACTIVITY_DELAY);
    activityDelayEdit->setMaximum(MAX_ACTIVITY_DELAY);
    activityDelayEdit->setValue(settings._activityTime / 60000);
    //validation
    auto okValidation = [&]()
    {
        ok->setDisabled(nameEdit->validate() != Wt::ValidationState::Valid ||
                        periodEdit->validate() != Wt::ValidationState::Valid ||
                        activityDelayEdit->validate() != Wt::ValidationState::Valid);
    };
    nameEdit->keyWentUp().connect(okValidation);
    periodEdit->valueChanged().connect(okValidation);
    periodEdit->keyWentUp().connect(okValidation);
    activityDelayEdit->valueChanged().connect(okValidation);
    activityDelayEdit->keyWentUp().connect(okValidation);
    okValidation();
    //execute
    if (dialog->exec() != DialogCode::Accepted)
        return;    
    //update name
    auto newName = nameEdit->text().toUTF8();
    if (newName.size())
    {
        DeviceName deviceName;
        deviceName._name = newName;
        auto result = RequestHelper::DoPostRequest({ "127.0.0.1", _settings._servicePort, UrlHelper::Url(API_CLIENT_DEVICE_NAME, "<string>", _deviceId.data()) }, Constants::LoginService, deviceName.ToJson());
        if (result == 200)
        {
            _deviceName = newName;
            _nameText->setText(WidgetHelper::TextWithFontSize(_deviceName, 20));
        }
        else
            LOG_ERROR << "Failed to update name to " << newName << "." << std::endl;
    }
    //update settings
    MotionRelaySettings newSettings;
    newSettings._period = periodEdit->value() * 1000;
    newSettings._activityTime = activityDelayEdit->value() * 60000;
    auto result = RequestHelper::DoPostRequest({ "127.0.0.1", _settings._servicePort, UrlHelper::Url(API_DEVICE_SETTINGS, "<string>", _deviceId.data()) }, Constants::LoginService, newSettings.ToJson());
    if (result != 200)
        LOG_ERROR << "Failed to update settings to " << newSettings.ToJson().dump() << "." << std::endl;
}

void MotionRelayWidget::OnStateButton()
{
    auto newState = _deviceState ? 0 : 1;
    //update commands
    RelayState newCommands;
    newCommands._state = newState;
    auto result = RequestHelper::DoPostRequest({ "127.0.0.1", _settings._servicePort, UrlHelper::Url(API_DEVICE_COMMANDS, "<string>", _deviceId.data()) }, Constants::LoginService, newCommands.ToJson());
    if (result != 200)
        LOG_ERROR << "Failed to update commands to " << newCommands.ToJson().dump() << "." << std::endl;
    _stateButton->setEnabled(false);
}
