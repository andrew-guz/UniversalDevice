#include "RelayWidget.hpp"

#include <algorithm>
#include <memory>

#include <Wt/Chart/WCartesianChart.h>
#include <Wt/WEvent.h>
#include <Wt/WGlobal.h>
#include <Wt/WTimer.h>
#include <fmt/format.h>

#include "ApplicationSettings.hpp"
#include "ChartFactory.hpp"
#include "Constants.hpp"
#include "Defines.hpp"
#include "Logger.hpp"
#include "PeriodSettings.hpp"
#include "RelayState.hpp"
#include "WidgetHelper.hpp"

using namespace Wt;

RelayWidget::RelayWidget(IStackHolder* stackHolder, const ApplicationSettings& settings) :
    BaseDeviceWidget(stackHolder, settings) {
    _stateText = _mainLayout->addWidget(std::make_unique<WText>(), 0, AlignmentFlag::Center | AlignmentFlag::Top);
    _stateText->setText(WidgetHelper::TextWithFontSize("Выключено", 80));

    _stateButton = _mainLayout->addWidget(std::make_unique<WPushButton>(), 0, AlignmentFlag::Center | AlignmentFlag::Top);
    _stateButton->setTextFormat(TextFormat::XHTML);
    _stateButton->setText(WidgetHelper::TextWithFontSize("Включить", 32));
    _stateButton->setMinimumSize(200, 200);
    _stateButton->setMaximumSize(200, 200);
    _stateButton->clicked().connect([this]() { OnStateButton(); });

    _model = std::make_shared<RelayChartModel>();
    _mainLayout->addWidget(
        std::unique_ptr<Chart::WCartesianChart>(ChartFactory::CreateChart(_model, false, Chart::SeriesType::Line, WColor(255, 0, 0, 255))), 1);
    _mainLayout->addWidget(std::make_unique<WText>("За последние:"), 0, AlignmentFlag::Center | AlignmentFlag::Bottom);
    _seconds = _mainLayout->addWidget(std::make_unique<SecondsComboBox>(), 0, AlignmentFlag::Center | AlignmentFlag::Bottom);
    _seconds->changed().connect([this]() {
        _cachedValues.clear();
        BaseDeviceWidget::Initialize(_deviceId.data());
    });
}

void RelayWidget::OnBack() {
    _seconds->setCurrentIndex(1);
    _cachedValues.clear();
    BaseDeviceWidget::OnBack();
}

void RelayWidget::Initialize() {
    const uint64_t seconds = _seconds->GetSeconds();
    if (_cachedValues.empty())
        _cachedValues = GetValues<ExtendedRelayCurrentState>(DeviceType::Relay, seconds);
    else {
        auto lastValues = GetValues<ExtendedRelayCurrentState>(DeviceType::Relay, 0);
        if (lastValues.empty() ||
            lastValues[0]._timestamp != std::max_element(_cachedValues.begin(), _cachedValues.end(), [](const auto& a, const auto& b) {
                                            return a._timestamp < b._timestamp;
                                        })->_timestamp)
            _cachedValues = GetValues<ExtendedRelayCurrentState>(DeviceType::Relay, seconds);
    }
    if (_cachedValues.size()) {
        _deviceState = _cachedValues.begin()->_state;
        const int activeCount =
            std::count_if(_cachedValues.begin(), _cachedValues.end(), [](const ExtendedRelayCurrentState& state) -> bool { return state._state; });
        const std::string text =
            fmt::format("{} ({:.2f})", _deviceState ? "Включено" : "Выключено", (float)activeCount / (float)_cachedValues.size());
        _stateText->setText(WidgetHelper::TextWithFontSize(text, 80));
        _stateButton->setText(WidgetHelper::TextWithFontSize(_deviceState ? "Выключить" : "Включить", 32));
        auto timestamp = _cachedValues.begin()->_timestamp;
        _timeText->setText(WidgetHelper::TextWithFontSize(TimeHelper::TimeToString(timestamp), 20));
        _model->UpdateData(_cachedValues);
    } else
        Clear(BaseDeviceWidget::ClearType::Data);
    _stateButton->setEnabled(true);
}

void RelayWidget::ClearData() {
    _timeText->setText(WidgetHelper::TextWithFontSize("", 20));
    _stateText->setText(WidgetHelper::TextWithFontSize("Выключено", 80));
    _stateButton->setText(WidgetHelper::TextWithFontSize("Включить", 32));
    _model->UpdateData({});
}

void RelayWidget::OnSettingsButton() {
    if (_deviceId.isEmpty())
        return;
    auto settings = GetSettings<PeriodSettings>();
    auto [dialog, layout, nameEdit, groupEdit, periodEdit, ok] = WidgetHelper::CreateBaseSettingsDialog(
        this, 180, _deviceName, _deviceGroup, settings._period, true, std::bind(&RelayWidget::onRestart, this));
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
                                     Constants::LoginService,
                                     newSettings);
    if (result != 200)
        LOG_ERROR_MSG(fmt::format("Failed to update settings to {}", nlohmann::json(newSettings).dump()));
}

void RelayWidget::OnStateButton() {
    auto newState = _deviceState ? 0 : 1;
    // update commands
    RelayState newCommands;
    newCommands._state = newState;
    auto result =
        RequestHelper::DoPostRequest({ BACKEND_IP, _settings._servicePort, UrlHelper::Url(API_DEVICE_COMMANDS, "<string>", _deviceId.data()) },
                                     Constants::LoginService,
                                     newCommands);
    if (result != 200)
        LOG_ERROR_MSG(fmt::format("Failed to update commands to {}", nlohmann::json(newCommands).dump()));
    _stateButton->setEnabled(false);
}
