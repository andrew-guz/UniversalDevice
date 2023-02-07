#include "ThermometerWidget.h"

#include <Wt/WPushButton.h>
#include <Wt/WTimer.h>
#include <Wt/WDialog.h>
#include <Wt/WLineEdit.h>
#include <Wt/WSpinBox.h>
#include <Wt/WRegExpValidator.h>

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
    BaseStackWidget(stackHolder, settings),
    _deviceId(Uuid::Empty())
{
    _mainLayout = setLayout(std::make_unique<WGridLayout>());

    auto backButton = _mainLayout->addWidget(std::make_unique<WPushButton>(), 0, 0, AlignmentFlag::Left);
    WidgetHelper::SetUsualButtonSize(backButton);
    backButton->setText("Назад...");
    backButton->clicked().connect([&](){ _stackHolder->SetWidget(StackWidgetType::Devices, ""); });

    auto settingsButton = _mainLayout->addWidget(std::make_unique<WPushButton>(), 0, 1, AlignmentFlag::Center);
    WidgetHelper::SetUsualButtonSize(settingsButton);
    settingsButton->setText("Настройки");
    settingsButton->clicked().connect([&](){ OnSettings(); });

    auto refresh = _mainLayout->addWidget(std::make_unique<WPushButton>(), 0, 2, AlignmentFlag::Right);
    WidgetHelper::SetUsualButtonSize(refresh);
    refresh->setText("Обновить...");
    refresh->clicked().connect([&](){ Initialize(_deviceId.data()); });

    _nameText = _mainLayout->addWidget(std::make_unique<WText>(), 1, 1, AlignmentFlag::Center);
    _nameText->setText(WidgetHelper::TextWithFontSize(_thermometerName, 20));

    _temperatureText = _mainLayout->addWidget(std::make_unique<WText>(), 2, 1, AlignmentFlag::Center);
    _temperatureText->setText(WidgetHelper::TextWithFontSize(0.0f, "°C", 80));

    _temperatureTimeText = _mainLayout->addWidget(std::make_unique<WText>(), 3, 1, AlignmentFlag::Center);
    _temperatureTimeText->setText(WidgetHelper::TextWithFontSize("", 20));
    
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

    auto refreshTimer = addChild(std::make_unique<WTimer>());
    refreshTimer->setInterval(std::chrono::seconds(5));
    refreshTimer->timeout().connect([&](){ Initialize(_deviceId.data()); });
    refreshTimer->start();
}

void ThermometerWidget::Initialize(const std::string& data)
{
    std::lock_guard<std::mutex> lock_guard(_requestMutex);

    _deviceId =  Uuid(data);
    if (_deviceId.isEmpty())
    {
        Clear(ThermometerWidget::ClearType::All);
        return;
    }
    UpdateName();
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
        _temperatureTimeText->setText(WidgetHelper::TextWithFontSize(TimeHelper::TimeToString(timestamp), 20));
        _model->UpdateData(thermometerValues);
    }
    else
        Clear(ThermometerWidget::ClearType::Temperature);
}

void ThermometerWidget::Clear(ThermometerWidget::ClearType type)
{
    if (type & ThermometerWidget::ClearType::Name)
    {
        _thermometerName = {};
        _nameText->setText(WidgetHelper::TextWithFontSize(_thermometerName, 20));
    }
    if (type & ThermometerWidget::ClearType::Temperature)
    {
        _temperatureText->setText(WidgetHelper::TextWithFontSize(0.0f, "°C", 80));
        _temperatureTimeText->setText(WidgetHelper::TextWithFontSize("", 20));
        _model->UpdateData({});        
    }
}

void ThermometerWidget::UpdateName()
{
    auto replyJson = RequestHelper::DoGetRequest({ "127.0.0.1", _settings._servicePort, UrlHelper::Url(API_CLIENT_DEVICE_NAME, "<string>", _deviceId.data()) });
    _thermometerName = JsonExtension::CreateFromJson<DeviceName>(replyJson)._name;
    _nameText->setText(WidgetHelper::TextWithFontSize(_thermometerName, 20));
}

ThermometerSettings ThermometerWidget::GetSettings()
{
    auto replyJson = RequestHelper::DoGetRequest({ "127.0.0.1", _settings._servicePort, UrlHelper::Url(API_DEVICE_SETTINGS, "<string>", _deviceId.data()) });
    return JsonExtension::CreateFromJson<ThermometerSettings>(replyJson);
}

std::vector<ExtendedThermometerCurrentValue> ThermometerWidget::GetValues()
{
    ComponentDescription messageData;
    messageData._type = Constants::DeviceTypeThermometer;
    messageData._id = _deviceId;
    auto postMessage = MessageHelper::Create({}, Uuid::Empty(), Constants::SubjectGetDeviceInformation, messageData.ToJson());
    auto replyJson = RequestHelper::DoPostRequestWithAnswer({ "127.0.0.1", _settings._servicePort, API_CLIENT_DEVICE_GET_INFO }, postMessage.ToJson());
    return JsonExtension::CreateVectorFromJson<ExtendedThermometerCurrentValue>(replyJson);
}

void ThermometerWidget::OnSettings()
{
    if (_deviceId.isEmpty())
        return;

    auto dialog = addChild(std::make_unique<WDialog>("Настройки"));
    auto layout = dialog->contents()->setLayout(std::make_unique<WGridLayout>());
    dialog->setMinimumSize(400, 150);
    dialog->setClosable(true);
    dialog->setResizable(false);
    dialog->rejectWhenEscapePressed(true);
    //name
    layout->addWidget(std::make_unique<WText>("Имя:"), 0, 0);
    auto validator = std::make_shared<WRegExpValidator>("[\\w\\s\u0401\u0451\u0400-\u04ff]{0,50}");
    validator->setMandatory(true);
    auto nameEdit = layout->addWidget(std::make_unique<WLineEdit>(), 0, 1);
    nameEdit->setValidator(validator);
    nameEdit->setText(_thermometerName);
    nameEdit->setFocus();
    //period
    layout->addWidget(std::make_unique<WText>("Период обновления (сек):"), 1, 0);
    auto period = layout->addWidget(std::make_unique<WSpinBox>(), 1, 1);
    period->setMinimum(1);
    period->setMaximum(600);
    period->setValue(GetSettings()._period / 1000);
    //ok button
    auto ok = dialog->footer()->addWidget(std::make_unique<WPushButton>("Ok"));
    ok->setDefault(true);
    ok->clicked().connect(dialog, &WDialog::accept); 
    auto okValidation = [=]()
    {
        ok->setDisabled(nameEdit->validate() != Wt::ValidationState::Valid ||
                        period->validate() != Wt::ValidationState::Valid);
    };
    nameEdit->keyWentUp().connect(okValidation);
    period->valueChanged().connect(okValidation);
    period->keyWentUp().connect(okValidation);
    okValidation();
    if (dialog->exec() != DialogCode::Accepted)
        return;
    //update name
    auto newName = nameEdit->text().toUTF8();
    if (newName.size())
    {
        DeviceName deviceName;
        deviceName._name = newName;
        auto result = RequestHelper::DoPostRequestWithNoAnswer({ "127.0.0.1", _settings._servicePort, UrlHelper::Url(API_CLIENT_DEVICE_NAME, "<string>", _deviceId.data()) }, deviceName.ToJson());
        if (result == 200)
        {
            _thermometerName = newName;
            _nameText->setText(WidgetHelper::TextWithFontSize(_thermometerName, 20));
        }
        else
            LOG_ERROR << "Failed to update name to " << newName << "." << std::endl;
    }
    //update settings
    ThermometerSettings newSettings;
    newSettings._period = period->value() * 1000;
    auto result = RequestHelper::DoPostRequestWithNoAnswer({ "127.0.0.1", _settings._servicePort, UrlHelper::Url(API_DEVICE_SETTINGS, "<string>", _deviceId.data()) }, newSettings.ToJson());
    if (result != 200)
        LOG_ERROR << "Failed to update settings to " << newSettings.ToJson().dump() << "." << std::endl;
}
