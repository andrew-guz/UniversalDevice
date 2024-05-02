#include "RelayWidget.hpp"

#include <Wt/WEvent.h>
#include <Wt/WTimer.h>

#include "ComponentDescription.hpp"
#include "Constants.hpp"
#include "Defines.hpp"
#include "DeviceProperty.hpp"
#include "ExtendedComponentDescription.hpp"
#include "Logger.hpp"
#include "RelayState.hpp"
#include "WidgetHelper.hpp"

using namespace Wt;

RelayWidget::RelayWidget(IStackHolder* stackHolder, const Settings& settings) : BaseDeviceWidget(stackHolder, settings) {
    _stateText = _mainLayout->addWidget(std::make_unique<WText>(), 3, 1, AlignmentFlag::Center);
    _stateText->setText(WidgetHelper::TextWithFontSize("Выключено", 80));

    _stateButton = _mainLayout->addWidget(std::make_unique<WPushButton>(), 4, 1, AlignmentFlag::Center);
    _stateButton->setTextFormat(TextFormat::XHTML);
    _stateButton->setText(WidgetHelper::TextWithFontSize("Включить", 32));
    _stateButton->setMinimumSize(200, 200);
    _stateButton->setMaximumSize(200, 200);
    _stateButton->clicked().connect([this]() { OnStateButton(); });

    _mainLayout->setRowStretch(3, 1);
    _mainLayout->setRowStretch(4, 1);
}

void RelayWidget::Initialize() {
    auto stateValues = GetValues<ExtendedRelayCurrentState>(Constants::DeviceTypeRelay);
    if (stateValues.size()) {
        _deviceState = stateValues.begin()->_state;
        _stateText->setText(WidgetHelper::TextWithFontSize(_deviceState ? "Включено" : "Выключено", 80));
        _stateButton->setText(WidgetHelper::TextWithFontSize(_deviceState ? "Выключить" : "Включить", 32));
        auto timestamp = stateValues.begin()->_timestamp;
        _timeText->setText(WidgetHelper::TextWithFontSize(TimeHelper::TimeToString(timestamp), 20));
    } else
        Clear(BaseDeviceWidget::ClearType::Data);
    _stateButton->setEnabled(true);
}

void RelayWidget::ClearData() {
    _timeText->setText(WidgetHelper::TextWithFontSize("", 20));
    _stateText->setText(WidgetHelper::TextWithFontSize("Выключено", 80));
    _stateButton->setText(WidgetHelper::TextWithFontSize("Включить", 32));
}

void RelayWidget::OnSettingsButton() {
    if (_deviceId.isEmpty())
        return;
    auto settings = GetSettings<PeriodSettings>();
    auto [dialog, layout, nameEdit, groupEdit, periodEdit, ok] =
        WidgetHelper::CreateBaseSettingsDialog(this, 180, _deviceName, _deviceGroup, settings._period, true);
    // execute
    if (dialog->exec() != DialogCode::Accepted)
        return;
    // update name
    auto newName = nameEdit->text().toUTF8();
    if (newName.size())
        SetNewName(newName);
    // update group
    SetNewGroup(groupEdit->text().toUTF8());
    // update settings
    PeriodSettings newSettings;
    newSettings._period = periodEdit->value() * 1000;
    auto result =
        RequestHelper::DoPostRequest({ BACKEND_IP, _settings._servicePort, UrlHelper::Url(API_DEVICE_SETTINGS, "<string>", _deviceId.data()) },
                                     Constants::LoginService, nlohmann::json{ newSettings });
    if (result != 200)
        LOG_ERROR << "Failed to update settings to " << nlohmann::json{ newSettings }.dump() << "." << std::endl;
}

void RelayWidget::OnStateButton() {
    auto newState = _deviceState ? 0 : 1;
    // update commands
    RelayState newCommands;
    newCommands._state = newState;
    auto result =
        RequestHelper::DoPostRequest({ BACKEND_IP, _settings._servicePort, UrlHelper::Url(API_DEVICE_COMMANDS, "<string>", _deviceId.data()) },
                                     Constants::LoginService, nlohmann::json{ newCommands });
    if (result != 200)
        LOG_ERROR << "Failed to update commands to " << nlohmann::json{ newCommands }.dump() << "." << std::endl;
    _stateButton->setEnabled(false);
}
