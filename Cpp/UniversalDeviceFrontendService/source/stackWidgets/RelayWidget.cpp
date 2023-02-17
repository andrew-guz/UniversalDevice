#include "RelayWidget.h"

#include <Wt/WTimer.h>
#include <Wt/WEvent.h>

#include "Defines.h"
#include "Constants.h"
#include "Logger.h"
#include "ComponentDescription.h"
#include "MessageHelper.h"
#include "RequestHelper.h"
#include "UrlHelper.h"
#include "ExtendedComponentDescription.h"
#include "DeviceName.h"
#include "WidgetHelper.h"

using namespace Wt;

RelayWidget::RelayWidget(IStackHolder* stackHolder, const Settings& settings) :
    BaseDeviceWidget(stackHolder, settings)
{
    _stateText = _mainLayout->addWidget(std::make_unique<WText>(), 3, 1, AlignmentFlag::Center);
    _stateText->setText(WidgetHelper::TextWithFontSize("Выключено", 80));

    _stateButton = _mainLayout->addWidget(std::make_unique<WPushButton>(), 4, 1, AlignmentFlag::Center);
    _stateButton->setTextFormat(TextFormat::XHTML);
    _stateButton->setText(WidgetHelper::TextWithFontSize("Включить", 32));
    _stateButton->setMinimumSize(200, 200);
    _stateButton->setMaximumSize(200, 200);
    _stateButton->clicked().connect([&]()
    {
        OnStateButton();
    });
}

void RelayWidget::Initialize()
{
    auto stateValues = GetValues();
    if (stateValues.size())
    {
        _deviceState = stateValues.begin()->_state;
        _stateText->setText(WidgetHelper::TextWithFontSize(_deviceState ? "Включено" : "Выключено", 80));
        _stateButton->setText(WidgetHelper::TextWithFontSize(_deviceState ? "Выключить" : "Включить", 32));
        auto timestamp = stateValues.begin()->_timestamp;
        _timeText->setText(WidgetHelper::TextWithFontSize(TimeHelper::TimeToString(timestamp), 20));
    }
    else
        Clear(RelayWidget::ClearType::Data);
    _stateButton->setEnabled(true);
}

void RelayWidget::ClearData()
{
    _timeText->setText(WidgetHelper::TextWithFontSize("", 20));
    _stateText->setText(WidgetHelper::TextWithFontSize("Выключено", 80));
    _stateButton->setText(WidgetHelper::TextWithFontSize("Включить", 32));
}

RelaySettings RelayWidget::GetSettings()
{
    auto replyJson = RequestHelper::DoGetRequest({ "127.0.0.1", _settings._servicePort, UrlHelper::Url(API_DEVICE_SETTINGS, "<string>", _deviceId.data()) }, Constants::LoginService);
    return JsonExtension::CreateFromJson<RelaySettings>(replyJson);
}

std::vector<ExtendedRelayCurrentState> RelayWidget::GetValues()
{
    ComponentDescription messageData;
    messageData._type = Constants::DeviceTypeRelay;
    messageData._id = _deviceId;
    auto postMessage = MessageHelper::Create({}, Uuid::Empty(), Constants::SubjectGetDeviceInformation, messageData.ToJson());
    auto replyJson = RequestHelper::DoPostRequestWithAnswer({ "127.0.0.1", _settings._servicePort, API_CLIENT_DEVICE_GET_INFO }, Constants::LoginService, postMessage.ToJson());
    return JsonExtension::CreateVectorFromJson<ExtendedRelayCurrentState>(replyJson);
}

void RelayWidget::OnSettingsButton()
{
    if (_deviceId.isEmpty())
        return;
    auto settings = GetSettings();
    auto [dialog, layout, nameEdit, periodEdit, ok] = WidgetHelper::CreateNamePeriodSettingsDialog(this, 150, _deviceName, settings._period, true);
    //execute
    if (dialog->exec() != DialogCode::Accepted)
        return;    
    //update name
    auto newName = nameEdit->text().toUTF8();
    if (newName.size())
    {
        DeviceName deviceName;
        deviceName._name = newName;
        auto result = RequestHelper::DoPostRequestWithNoAnswer({ "127.0.0.1", _settings._servicePort, UrlHelper::Url(API_CLIENT_DEVICE_NAME, "<string>", _deviceId.data()) }, Constants::LoginService, deviceName.ToJson());
        if (result == 200)
        {
            _deviceName = newName;
            _nameText->setText(WidgetHelper::TextWithFontSize(_deviceName, 20));
        }
        else
            LOG_ERROR << "Failed to update name to " << newName << "." << std::endl;
    }
    //update settings
    RelaySettings newSettings;
    newSettings._period = periodEdit->value() * 1000;
    auto result = RequestHelper::DoPostRequestWithNoAnswer({ "127.0.0.1", _settings._servicePort, UrlHelper::Url(API_DEVICE_SETTINGS, "<string>", _deviceId.data()) }, Constants::LoginService, newSettings.ToJson());
    if (result != 200)
        LOG_ERROR << "Failed to update settings to " << newSettings.ToJson().dump() << "." << std::endl;
}

void RelayWidget::OnStateButton()
{
    auto newState = _deviceState ? 0 : 1;
    //update commands
    RelayCurrentState newCommands;
    newCommands._state = newState;
    auto result = RequestHelper::DoPostRequestWithNoAnswer({ "127.0.0.1", _settings._servicePort, UrlHelper::Url(API_DEVICE_COMMANDS, "<string>", _deviceId.data()) }, Constants::LoginService, newCommands.ToJson());
    if (result != 200)
        LOG_ERROR << "Failed to update commands to " << newCommands.ToJson().dump() << "." << std::endl;
    _stateButton->setEnabled(false);
}
