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

using namespace Wt;

ThermometerWidget::ThermometerWidget(IStackHolder* stackHolder, const Settings& settings) :
    BaseStackWidget(stackHolder, settings)
{
    _mainLayout = setLayout(std::make_unique<WVBoxLayout>());
    auto backButton = _mainLayout->addWidget(std::make_unique<WPushButton>());
    backButton->setText("Назад...");
    backButton->clicked().connect([&](){ _stackHolder->SetWidget(StackWidgetType::Devices, ""); });
    auto refresh = _mainLayout->addWidget(std::make_unique<WPushButton>());
    refresh->setText("Обновить");
    refresh->clicked().connect([&](){ Initialize(_deviceId.data()); });
    _mainText = _mainLayout->addWidget(std::make_unique<WText>(), 0, AlignmentFlag::Center);
    _mainText->setText("0");

}

void ThermometerWidget::Initialize(const std::string& data)
{   
    _deviceId =  Uuid(data);
    ComponentDescription messageData;
    messageData._type = Constants::DeviceTypeThermometer;
    messageData._id = _deviceId;
    auto postMessage = MessageHelper::Create(Constants::FrontendServiceType, Uuid::Empty(), Constants::ClientServiceType, Uuid::Empty(), Constants::SubjectGetDeviceInformation, messageData.ToJson());
    auto replyMessage = RequestHelper::DoPostRequestWithAnswer({ "127.0.0.1", _settings._servicePort, API_CLIENT_DEVICE }, postMessage);
    auto thermometerValues = MessageHelper::ParseMessage<ExtendedThermometerCurrentValue>(replyMessage);
    if (!thermometerValues.size())
        return;
    _mainText->setText(std::to_string(thermometerValues.begin()->_value));
}
