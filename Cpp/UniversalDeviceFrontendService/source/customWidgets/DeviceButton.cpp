#include "DeviceButton.h"

#include "Defines.h"
#include "FrontendDefines.h"
#include "JsonExtension.h"
#include "DeviceInformationDescription.h"
#include "MessageHelper.h"
#include "RequestHelper.h"
#include "WidgetHelper.h"
#include "ExtendedThermometerCurrentValue.h"
#include "ExtendedRelayCurrentState.h"
#include "ExtendedMotionRelayCurrentState.h"

using namespace Wt;

DeviceButton::DeviceButton(int port, const ExtendedComponentDescription& description) :
    WPushButton(),
    _port(port),
    _deviceType(description._type),
    _deviceId(description._id),
    _text(description.GetDisplayString())
{
    setText(_text);
    setMinimumSize(200, 150);
    setMaximumSize(200, 150);

    _refreshTimer = addChild(std::make_unique<WTimer>());
    _refreshTimer->setInterval(std::chrono::seconds(5));
    _refreshTimer->timeout().connect([&](){
        Refresh();
    });
    _refreshTimer->start();
    Refresh();
}

DeviceButton::~DeviceButton()
{
    _refreshTimer->stop();
}

void DeviceButton::Refresh()
{
    auto text = _text;
    DeviceInformationDescription messageData;
    messageData._type = _deviceType;
    messageData._id = _deviceId;
    messageData._seconds = 0;
    auto postMessage = MessageHelper::Create({}, Uuid::Empty(), Constants::SubjectGetDeviceInformation, messageData.ToJson());
    auto replyJson = RequestHelper::DoPostRequestWithAnswer({ BACKEND_IP, _port, API_CLIENT_DEVICE_GET_INFO }, Constants::LoginService, postMessage.ToJson());
    if (!replyJson.is_null())
    {
        std::string additionalData;
        std::chrono::system_clock::time_point timestamp;
        if (_deviceType == Constants::DeviceTypeThermometer)
        {
            auto values = JsonExtension::CreateVectorFromJson<ExtendedThermometerCurrentValue>(replyJson);
            if (values.size())
            {
                const auto& value = values[0];
                std::stringstream ss;
                ss << WidgetHelper::TextWithFontSize("🌡", 40);
                ss.precision(1);
                ss << std::fixed << value._value << "°C";
                ss.flush();
                additionalData = ss.str();
                timestamp = value._timestamp;
            }
        }
        else if (_deviceType == Constants::DeviceTypeRelay)
        {
            auto values = JsonExtension::CreateVectorFromJson<ExtendedRelayCurrentState>(replyJson);
            if (values.size())
            {
                const auto& value = values[0];
                additionalData = WidgetHelper::TextWithFontSize("⏻", 40) + (value._state == 1 ? "ON" : "OFF");
                timestamp = value._timestamp;
            }
        }
        else if (_deviceType == Constants::DeviceTypeMotionRelay)
        {
            auto values = JsonExtension::CreateVectorFromJson<ExtendedMotionRelayCurrentState>(replyJson);
            if (values.size())
            {
                const auto& value = values[0];
                additionalData = WidgetHelper::TextWithFontSize("⏻", 40) + (value._motion == 1 ? "Движение" : "...");
                timestamp = value._timestamp;
            }
        }
        else
            LOG_ERROR << "Unknown device type" << std::endl;
        if (additionalData.size())
            text += std::string("<br/><br/>") + additionalData + std::string("<br/><br/>") + TimeHelper::TimeToShortString(timestamp);
    }
    setTextFormat(TextFormat::XHTML);
    setText(text);
}
