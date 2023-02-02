#include "ThermometerWidget.h"

#include <Wt/WPushButton.h>
#include <Wt/WTimer.h>
#include <Wt/Chart/WCartesianChart.h>

#include "Defines.h"
#include "Constants.h"
#include "Logger.h"
#include "Uuid.h"
#include "ComponentDescription.h"
#include "MessageHelper.h"
#include "RequestHelper.h"
#include "ExtendedThermometerCurrentValue.h"
#include "WidgetHelper.h"

using namespace Wt;

ThermometerWidget::ThermometerWidget(IStackHolder* stackHolder, const Settings& settings) :
    BaseStackWidget(stackHolder, settings)
{
    _mainLayout = setLayout(std::make_unique<WGridLayout>());
    auto backButton = _mainLayout->addWidget(std::make_unique<WPushButton>(), 0, 0, AlignmentFlag::Left);
    WidgetHelper::SetUsualButtonSize(backButton);
    backButton->setText("Назад...");
    backButton->clicked().connect([&](){ _stackHolder->SetWidget(StackWidgetType::Devices, ""); });
    backButton->setMaximumSize(backButton->maximumWidth(), 50);
    auto refresh = _mainLayout->addWidget(std::make_unique<WPushButton>(), 0, 3, AlignmentFlag::Right);
    WidgetHelper::SetUsualButtonSize(refresh);
    refresh->setText("Обновить...");
    refresh->clicked().connect([&](){ Initialize(_deviceId.data()); });
    refresh->setMaximumSize(refresh->maximumWidth(), 50);
    _mainText = _mainLayout->addWidget(std::make_unique<WText>(), 1, 2, AlignmentFlag::Center);
    _mainText->setText(WidgetHelper::TextWithFontSize(0.0f, 80));
    auto refreshTimer = addChild(std::make_unique<WTimer>());
    refreshTimer->setInterval(std::chrono::seconds(5));
    refreshTimer->timeout().connect([&](){ Initialize(_deviceId.data()); });
    refreshTimer->start();
    _model = std::make_shared<TemperatureChartModel>();
    auto chart = _mainLayout->addWidget(std::make_unique<Chart::WCartesianChart>(), 2, 2, AlignmentFlag::Center);
    chart->resize(800, 600);
    chart->setModel(_model);
    chart->setXSeriesColumn(0);
    chart->setType(Chart::ChartType::Scatter);
    auto series = std::make_unique<Chart::WDataSeries>(1, Chart::SeriesType::Curve, Chart::Axis::Y);
    series->setPen(Wt::WPen(WColor(0, 0, 255, 255)));
    series->setShadow(Wt::WShadow(3, 3, WColor(0, 0, 0, 127), 3));
    chart->addSeries(std::move(series));
    chart->axis(Chart::Axis::X).setScale(Chart::AxisScale::DateTime);
    chart->axis(Chart::Axis::Y).setMinimum(-20.0);
    chart->axis(Chart::Axis::Y).setMaximum(40.0);
}

void ThermometerWidget::Initialize(const std::string& data)
{       
    _deviceId =  Uuid(data);
    if (_deviceId.isEmpty())
    {
        Clear();
        return;
    }
    ComponentDescription messageData;
    messageData._type = Constants::DeviceTypeThermometer;
    messageData._id = _deviceId;
    auto postMessage = MessageHelper::Create(Constants::FrontendServiceType, Uuid::Empty(), Constants::ClientServiceType, Uuid::Empty(), Constants::SubjectGetDeviceInformation, messageData.ToJson());
    auto replyJson = RequestHelper::DoPostRequestWithAnswer({ "127.0.0.1", _settings._servicePort, API_CLIENT_DEVICE }, postMessage.ToJson());
    auto thermometerValues = JsonExtension::CreateVectorFromJson<ExtendedThermometerCurrentValue>(replyJson);
    if (thermometerValues.size())
    {
        _mainText->setText(WidgetHelper::TextWithFontSize(thermometerValues.begin()->_value, 80));
        _model->UpdateData(thermometerValues);
    }
    else
        Clear();
}

void ThermometerWidget::Clear()
{
    _mainText->setText(WidgetHelper::TextWithFontSize(0.0f, 80));
    _model->UpdateData({});
}
