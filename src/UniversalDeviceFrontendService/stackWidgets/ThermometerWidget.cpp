#include "ThermometerWidget.hpp"

#include <memory>

#include <fmt/format.h>

#include "BaseDeviceWidget.hpp"
#include "ChartFactory.hpp"
#include "Constants.hpp"
#include "Defines.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "SecondsComboBox.hpp"
#include "WidgetHelper.hpp"

using namespace Wt;

ThermometerWidget::ThermometerWidget(IStackHolder* stackHolder, const Settings& settings) :
    BaseDeviceWidget(stackHolder, settings) {
    _temperatureText = _mainLayout->addWidget(std::make_unique<WText>(), 3, 0, 1, 3, AlignmentFlag::Center);
    _temperatureText->setText(WidgetHelper::TextWithFontSize(0.0f, "°C", 80));
    _temperatureText->setMaximumSize(400, 200);

    _model = std::make_shared<TemperatureChartModel>();
    _chart = _mainLayout->addWidget(
        std::unique_ptr<Chart::WCartesianChart>(ChartFactory::CreateChart(_model, true, Chart::SeriesType::Curve, WColor(0, 0, 255, 255))),
        4,
        0,
        1,
        3);
    _mainLayout->addWidget(std::make_unique<WText>("За последние:"), 5, 1, AlignmentFlag::Center);
    _seconds = _mainLayout->addWidget(std::make_unique<SecondsComboBox>(), 6, 1, AlignmentFlag::Center);
    _seconds->changed().connect([this]() {
        _cachedValues.clear();
        BaseDeviceWidget::Initialize(_deviceId.data());
    });

    _mainLayout->setRowStretch(3, 0);
    _mainLayout->setRowStretch(4, 1);
    _mainLayout->setRowStretch(5, 0);
    _mainLayout->setRowStretch(6, 0);
}

void ThermometerWidget::OnBack() {
    _seconds->setCurrentIndex(1);
    _cachedValues.clear();
    BaseDeviceWidget::OnBack();
}

void ThermometerWidget::Initialize() {
    const uint64_t seconds = _seconds->GetSeconds();
    if (_cachedValues.empty())
        _cachedValues = GetValues<ExtendedThermometerCurrentValue>(DeviceType::Thermometer, seconds);
    else {
        auto lastValues = GetValues<ExtendedThermometerCurrentValue>(DeviceType::Thermometer, 0);
        if (lastValues.empty() ||
            lastValues[0]._timestamp != std::max_element(_cachedValues.begin(), _cachedValues.end(), [](const auto& a, const auto& b) {
                                            return a._timestamp < b._timestamp;
                                        })->_timestamp)
            _cachedValues = GetValues<ExtendedThermometerCurrentValue>(DeviceType::Thermometer, seconds);
    }
    if (_cachedValues.size()) {
        auto value = _cachedValues.begin()->_value;
        _temperatureText->setText(WidgetHelper::TextWithFontSize(value, "°C", 80));
        auto min =
            std::min_element(_cachedValues.begin(), _cachedValues.end(), [](const auto& a, const auto& b) { return a._value < b._value; })->_value;
        auto max =
            std::max_element(_cachedValues.begin(), _cachedValues.end(), [](const auto& a, const auto& b) { return a._value < b._value; })->_value;
        _chart->axis(Chart::Axis::Y).setMinimum(min - 1);
        _chart->axis(Chart::Axis::Y).setMaximum(max + 1);
        auto timestamp = _cachedValues.begin()->_timestamp;
        _timeText->setText(WidgetHelper::TextWithFontSize(TimeHelper::TimeToString(timestamp), 20));
        _model->UpdateData(_cachedValues);
    } else
        Clear(BaseDeviceWidget::ClearType::Data);
}

void ThermometerWidget::ClearData() {
    _timeText->setText(WidgetHelper::TextWithFontSize("", 20));
    _temperatureText->setText(WidgetHelper::TextWithFontSize(0.0f, "°C", 80));
    _model->UpdateData({});
}

ThermometerLedBrightness ThermometerWidget::GetBrightness() {
    auto replyJson = RequestHelper::DoGetRequest(
        { BACKEND_IP, _settings._servicePort, UrlHelper::Url(API_DEVICE_COMMANDS, "<string>", _deviceId.data()) }, Constants::LoginService);
    return !replyJson.is_null() ? replyJson.get<ThermometerLedBrightness>() : ThermometerLedBrightness{};
}

void ThermometerWidget::OnSettingsButton() {
    if (_deviceId.isEmpty())
        return;
    auto [dialog, layout, nameEdit, groupEdit, periodEdit, ok] = WidgetHelper::CreateBaseSettingsDialog(
        this, 210, _deviceName, _deviceGroup, GetSettings<PeriodSettings>()._period, false, std::bind(&ThermometerWidget::onRestart, this));
    // brightness
    layout->addWidget(std::make_unique<WText>("Яркость:"), 3, 0);
    auto brightnessEdit = layout->addWidget(std::make_unique<WSpinBox>(), 3, 1);
    brightnessEdit->setMinimum(MIN_BRIGHTNESS);
    brightnessEdit->setMaximum(MAX_BRIGHTNESS);
    brightnessEdit->setValue(GetBrightness()._brightness);
    // validation
    auto okValidation = [&ok, &nameEdit, &periodEdit, &brightnessEdit]() {
        ok->setDisabled(nameEdit->validate() != Wt::ValidationState::Valid || periodEdit->validate() != Wt::ValidationState::Valid ||
                        brightnessEdit->validate() != Wt::ValidationState::Valid);
    };
    nameEdit->keyWentUp().connect(okValidation);
    groupEdit->keyWentUp().connect(okValidation);
    periodEdit->valueChanged().connect(okValidation);
    periodEdit->keyWentUp().connect(okValidation);
    brightnessEdit->valueChanged().connect(okValidation);
    brightnessEdit->keyWentUp().connect(okValidation);
    okValidation();
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
    auto settingsResult =
        RequestHelper::DoPostRequest({ BACKEND_IP, _settings._servicePort, UrlHelper::Url(API_DEVICE_SETTINGS, "<string>", _deviceId.data()) },
                                     Constants::LoginService,
                                     newSettings);
    if (settingsResult != 200)
        LOG_ERROR_MSG(fmt::format("Failed to update settings to {}.", nlohmann::json(newSettings).dump()));
    // set brightness command
    ThermometerLedBrightness newCommand;
    newCommand._brightness = brightnessEdit->value();
    auto commandResult =
        RequestHelper::DoPostRequest({ BACKEND_IP, _settings._servicePort, UrlHelper::Url(API_DEVICE_COMMANDS, "<string>", _deviceId.data()) },
                                     Constants::LoginService,
                                     newCommand);
    if (commandResult != 200)
        LOG_ERROR_MSG(fmt::format("Failed to update settings to {}", nlohmann::json(newCommand).dump()));
}
