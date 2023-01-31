#include "ThermometerWidget.h"

#include <Wt/WPushButton.h>

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
}

void ThermometerWidget::Initialize(const std::string& data)
{   
    _deviceId =  Uuid(data);
    ComponentDescription messageData;
    messageData._type = Constants::DeviceTypeThermometer;
    messageData._id = _deviceId;
    auto postMessage = MessageHelper::Create(Constants::FrontendServiceType, Uuid::Empty(), Constants::ClientServiceType, Uuid::Empty(), Constants::SubjectGetDeviceInformation, messageData.ToJson());
    auto replyJson = RequestHelper::DoPostRequestWithAnswer({ "127.0.0.1", _settings._servicePort, API_CLIENT_DEVICE }, postMessage.ToJson());
    auto thermometerValues = JsonExtension::CreateVectorFromJson<ExtendedThermometerCurrentValue>(replyJson);
    if (!thermometerValues.size())
        return;
    _mainText->setText(WidgetHelper::TextWithFontSize(thermometerValues.begin()->_value, 80));
}
