#include "ThermometerWidget.h"

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

ThermometerWidget::ThermometerWidget(IStackHolder* stackHolder, const Settings& settings) :
    BaseDeviceWidget(stackHolder, settings)
{
    _temperatureText = _mainLayout->addWidget(std::make_unique<WText>(), 3, 1, AlignmentFlag::Center);
    _temperatureText->setText(WidgetHelper::TextWithFontSize(0.0f, "°C", 80));
    
    _model = std::make_shared<TemperatureChartModel>();
    _chart = _mainLayout->addWidget(std::make_unique<Chart::WCartesianChart>(), 4, 1, AlignmentFlag::Center);
    _chart->resize(800, 600);
    _chart->setModel(_model);
    _chart->setXSeriesColumn(0);
    _chart->setType(Chart::ChartType::Scatter);
    _chart->axis(Chart::Axis::X).setScale(Chart::AxisScale::DateTime);
    _chart->axis(Chart::Axis::Y).setGridLinesEnabled(true);
    auto series = std::make_unique<Chart::WDataSeries>(1, Chart::SeriesType::Curve, Chart::Axis::Y);
    series->setPen(WPen(WColor(0, 0, 255, 255)));
    series->setShadow(WShadow(3, 3, WColor(0, 0, 0, 127), 3));
    _chart->addSeries(std::move(series));
}

void ThermometerWidget::Initialize()
{
    auto thermometerValues = GetValues();
    if (thermometerValues.size())
    {
        auto value = thermometerValues.begin()->_value;
        _temperatureText->setText(WidgetHelper::TextWithFontSize(value, "°C", 80));
        auto min = std::min_element(thermometerValues.begin(), thermometerValues.end(), [](const auto& a, const auto& b){ return a._value < b._value;})->_value;
        auto max = std::max_element(thermometerValues.begin(), thermometerValues.end(), [](const auto& a, const auto& b){ return a._value < b._value;})->_value;
        _chart->axis(Chart::Axis::Y).setMinimum(2 * min - max);
        _chart->axis(Chart::Axis::Y).setMaximum(2 * max - min);
        auto timestamp = thermometerValues.begin()->_timestamp;
        _timeText->setText(WidgetHelper::TextWithFontSize(TimeHelper::TimeToString(timestamp), 20));
        _model->UpdateData(thermometerValues);
    }
    else
        Clear(ThermometerWidget::ClearType::Data);
}

void ThermometerWidget::ClearData()
{
    _timeText->setText(WidgetHelper::TextWithFontSize("", 20));
    _temperatureText->setText(WidgetHelper::TextWithFontSize(0.0f, "°C", 80));
    _model->UpdateData({});        
}

ThermometerSettings ThermometerWidget::GetSettings()
{
    auto replyJson = RequestHelper::DoGetRequest({ "127.0.0.1", _settings._servicePort, UrlHelper::Url(API_DEVICE_SETTINGS, "<string>", _deviceId.data()) }, Constants::LoginService);
    return JsonExtension::CreateFromJson<ThermometerSettings>(replyJson);
}

std::vector<ExtendedThermometerCurrentValue> ThermometerWidget::GetValues()
{
    ComponentDescription messageData;
    messageData._type = Constants::DeviceTypeThermometer;
    messageData._id = _deviceId;
    auto postMessage = MessageHelper::Create({}, Uuid::Empty(), Constants::SubjectGetDeviceInformation, messageData.ToJson());
    auto replyJson = RequestHelper::DoPostRequestWithAnswer({ "127.0.0.1", _settings._servicePort, API_CLIENT_DEVICE_GET_INFO }, Constants::LoginService, postMessage.ToJson());
    return JsonExtension::CreateVectorFromJson<ExtendedThermometerCurrentValue>(replyJson);
}

void ThermometerWidget::OnSettingsButton()
{
    if (_deviceId.isEmpty())
        return;
    auto [dialog, layout, nameEdit, periodEdit] = WidgetHelper::CreateNamePeriodSettingsDialog(this, _deviceName, GetSettings()._period);
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
    ThermometerSettings newSettings;
    newSettings._period = periodEdit->value() * 1000;
    auto result = RequestHelper::DoPostRequestWithNoAnswer({ "127.0.0.1", _settings._servicePort, UrlHelper::Url(API_DEVICE_SETTINGS, "<string>", _deviceId.data()) }, Constants::LoginService, newSettings.ToJson());
    if (result != 200)
        LOG_ERROR << "Failed to update settings to " << newSettings.ToJson().dump() << "." << std::endl;
}
