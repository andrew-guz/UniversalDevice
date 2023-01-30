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
    _mainLayout = setLayout(std::make_unique<WGridLayout>());
    auto backButton = _mainLayout->addWidget(std::make_unique<WPushButton>(), 0, 2, AlignmentFlag::Right);
    backButton->setText("Назад...");
    backButton->clicked().connect([&](){ _stackHolder->SetWidget(StackWidgetType::Devices, ""); });
}

void ThermometerWidget::Initialize(const std::string& data)
{   
    ComponentDescription messageData;
    messageData._type = Constants::DeviceTypeThermometer;
    messageData._id = Uuid(data);
    auto postMessage = MessageHelper::Create(Constants::FrontendServiceType, Uuid::Empty(), Constants::ClientServiceType, Uuid::Empty(), Constants::SubjectGetDeviceInformation, messageData.ToJson());
    auto replyMessage = RequestHelper::DoPostRequestWithAnswer({ "127.0.0.1", _settings._servicePort, API_CLIENT_DEVICE }, postMessage);
    auto thermometerValues = MessageHelper::ParseMessage<ExtendedThermometerCurrentValue>(replyMessage);
    if (!thermometerValues.size())
        return;
}
