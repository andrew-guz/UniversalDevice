#include "DeviceButton.h"

#include "Defines.h"
#include "MessageHelper.h"
#include "RequestHelper.h"
#include "ExtendedThermometerCurrentValue.h"
#include "ExtendedRelayCurrentState.h"
#include "ExtendedMotionRelayCurrentState.h"

using namespace Wt;

DeviceButton::DeviceButton(int port, const ExtendedComponentDescription& description) :
    WPushButton(),
    _port(port),
    _deviceType(description._type),
    _deviceId(description._id),
    _text(description._name.size() ? description._name : description._id.data())
{
    setText(_text);
    setMinimumSize(200, 150);
    setMaximumSize(200, 150);

    _refreshTimer = addChild(std::make_unique<WTimer>());
    _refreshTimer->setInterval(std::chrono::seconds(5));
    _refreshTimer->timeout().connect([&]()
    {
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
    ComponentDescription messageData;
    messageData._type = _deviceType;
    messageData._id = _deviceId;
    auto postMessage = MessageHelper::Create({}, Uuid::Empty(), Constants::SubjectGetDeviceInformationSingle, messageData.ToJson());
    auto replyJson = RequestHelper::DoPostRequestWithAnswer({ "127.0.0.1", _port, API_CLIENT_DEVICE_GET_INFO }, Constants::LoginService, postMessage.ToJson());
    if (!replyJson.is_null())
    {
        std::string additionalData;
        if (_deviceType == Constants::DeviceTypeThermometer)
        {
            auto values = JsonExtension::CreateVectorFromJson<ExtendedThermometerCurrentValue>(replyJson);
            if (values.size())
            {
                std::stringstream ss;
                ss.precision(1);
                ss << std::fixed << values[0]._value << "°C";
                ss.flush();
                additionalData = ss.str();
            }
        }
        else if (_deviceType == Constants::DeviceTypeRelay)
        {
            auto values = JsonExtension::CreateVectorFromJson<ExtendedRelayCurrentState>(replyJson);
            if (values.size())
                additionalData = values[0]._state == 1 ? "ON" : "OFF";
        }
        else if (_deviceType == Constants::DeviceTypeMotionRelay)
        {
            auto values = JsonExtension::CreateVectorFromJson<ExtendedMotionRelayCurrentState>(replyJson);
            if (values.size())
                additionalData = values[0]._motion == 1 ? "Движение" : "...";
        }
        else
            LOG_ERROR << "Unknown device type" << std::endl;
        if (additionalData.size())
            text += "\n\n" + additionalData;
    }
    setText(text);
}
