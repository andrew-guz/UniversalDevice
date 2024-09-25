#include "ThermometerWidget.hpp"

#include <fmt/format.h>

#include "Constants.hpp"
#include "Defines.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "WidgetHelper.hpp"

using namespace Wt;

ThermometerWidget::ThermometerWidget(IStackHolder* stackHolder, const Settings& settings) :
    BaseDeviceWidget(stackHolder, settings) {
    _temperatureText = _mainLayout->addWidget(std::make_unique<WText>(), 3, 0, 1, 3, AlignmentFlag::Center);
    _temperatureText->setText(WidgetHelper::TextWithFontSize(0.0f, "°C", 80));
    _temperatureText->setMaximumSize(400, 200);

    _model = std::make_shared<TemperatureChartModel>();
    _chart = _mainLayout->addWidget(std::make_unique<Chart::WCartesianChart>(), 4, 0, 1, 3);
    _chart->setModel(_model);
    _chart->setXSeriesColumn(0);
    _chart->setType(Chart::ChartType::Scatter);
    _chart->axis(Chart::Axis::X).setScale(Chart::AxisScale::DateTime);
    _chart->axis(Chart::Axis::Y).setGridLinesEnabled(true);
    auto series = std::make_unique<Chart::WDataSeries>(1, Chart::SeriesType::Curve, Chart::Axis::Y);
    series->setPen(WPen(WColor(0, 0, 255, 255)));
    series->setShadow(WShadow(3, 3, WColor(0, 0, 0, 127), 3));
    _chart->addSeries(std::move(series));
    _mainLayout->addWidget(std::make_unique<WText>("За последние:"), 5, 1, AlignmentFlag::Center);
    _seconds = _mainLayout->addWidget(std::make_unique<WComboBox>(), 6, 1, AlignmentFlag::Center);
    _seconds->setMinimumSize(200, 50);
    _seconds->setMaximumSize(200, 50);
    _seconds->addItem("1 час");
    _seconds->addItem("2 часа");
    _seconds->addItem("12 часов");
    _seconds->addItem("1 день");
    _seconds->addItem("1 неделя");
    _seconds->setCurrentIndex(1);
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
}

void ThermometerWidget::Initialize() {
    uint64_t seconds = 7200;
    switch (_seconds->currentIndex()) {
        case 0:
            seconds = 3600;
            break;
        case 1:
            seconds = 7200;
            break;
        case 2:
            seconds = 43200;
            break;
        case 3:
            seconds = 86400;
            break;
        case 4:
            seconds = 604800;
            break;
    }
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
    auto [dialog, layout, nameEdit, groupEdit, periodEdit, ok] =
        WidgetHelper::CreateBaseSettingsDialog(this, 210, _deviceName, _deviceGroup, GetSettings<PeriodSettings>()._period, false);
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
