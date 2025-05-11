#include "DeviceButton.hpp"

#include <fmt/format.h>

#include "Defines.hpp"
#include "DeviceInformationDescription.hpp"
#include "Enums.hpp"
#include "ExtendedMotionRelayCurrentState.hpp"
#include "ExtendedRelayCurrentState.hpp"
#include "ExtendedThermometerCurrentValue.hpp"
#include "ExtendedUniversalDeviceCurrentValues.hpp"
#include "FrontendDefines.hpp"
#include "Marshaling.hpp"
#include "MessageHelper.hpp"
#include "RequestHelper.hpp"

using namespace Wt;

DeviceButton::DeviceButton(int port, const ExtendedComponentDescription& description) :
    WPushButton(),
    _port(port),
    _deviceType(description._type),
    _deviceId(description._id),
    _text(description.GetDisplayString()) {
    setText(_text);
    setMinimumSize(200, 150);
    setMaximumSize(200, 150);

    _refreshTimer = addChild(std::make_unique<WTimer>());
    _refreshTimer->setInterval(std::chrono::seconds(5));
    _refreshTimer->timeout().connect([this]() { Refresh(); });
    _refreshTimer->start();
    Refresh();
}

DeviceButton::~DeviceButton() { _refreshTimer->stop(); }

void DeviceButton::Refresh() {
    auto text = _text;
    DeviceInformationDescription messageData;
    messageData._type = _deviceType;
    messageData._id = _deviceId;
    messageData._seconds = 0;
    auto postMessage = MessageHelper::Create(ClientActor{}, Constants::PredefinedIdClient, Subject::GetDeviceInformation, messageData);
    auto replyJson = RequestHelper::DoPostRequestWithAnswer({ BACKEND_IP, _port, API_CLIENT_DEVICE_GET_INFO }, Constants::LoginService, postMessage);
    if (!replyJson.is_null()) {
        std::string additionalData;
        std::chrono::system_clock::time_point timestamp;
        if (std::holds_alternative<DeviceType>(_deviceType)) {
            switch (std::get<DeviceType>(_deviceType)) {
                case DeviceType::Undefined:
                    LOG_ERROR_MSG("Unknown device type");
                    break;
                case DeviceType::Thermometer: {
                    auto values = replyJson.get<std::vector<ExtendedThermometerCurrentValue>>();
                    if (values.size()) {
                        const auto& value = values[0];
                        additionalData = fmt::format(
                            "<img src='resources/thermometer-{}.png'/><br/><b>{:.1f}°C</b>", value._value > 0.0f ? "hot" : "cold", value._value);
                        timestamp = value._timestamp;
                    }
                } break;
                case DeviceType::Relay: {
                    auto values = replyJson.get<std::vector<ExtendedRelayCurrentState>>();
                    if (values.size()) {
                        const auto& value = values[0];
                        additionalData = fmt::format("<img src='resources/relay-{}.png'/>", value._state == 1 ? "on" : "off");
                        timestamp = value._timestamp;
                    }
                } break;
                case DeviceType::MotionRelay: {
                    auto values = replyJson.get<std::vector<ExtendedMotionRelayCurrentState>>();
                    if (values.size()) {
                        const auto& value = values[0];
                        additionalData = fmt::format("<img src='resources/motion-relay.png'/><br/>{}", value._motion == 1 ? "Движение" : "...");
                        timestamp = value._timestamp;
                    }
                } break;
                case DeviceType::UniversalDevice: {
                    auto values = replyJson.get<std::vector<ExtendedUniversalDeviceCurrentValues>>();
                    if (values.size()) {
                        const auto& value = values[0];
                        additionalData = "<img src='resources/universal-device.png'/>";
                        timestamp = value._timestamp;
                    }
                } break;
            }
        } else
            LOG_ERROR_MSG("Unknown device type");
        if (additionalData.size())
            text += std::string("<br/><br/>") + additionalData + std::string("<br/><br/>") + TimeHelper::TimeToShortString(timestamp);
        auto currentTime = std::chrono::system_clock::now();
        if (currentTime - timestamp > std::chrono::minutes(5))
            decorationStyle().setBorder(WBorder(Wt::BorderStyle::Solid, 2, Wt::StandardColor::Red));
        else if (currentTime - timestamp > std::chrono::hours(1))
            decorationStyle().setBorder(WBorder(Wt::BorderStyle::Solid, 2, Wt::StandardColor::DarkRed));
        else
            decorationStyle().setBorder(WBorder(Wt::BorderStyle::Solid, 1, Wt::StandardColor::DarkGray));
    }
    setTextFormat(TextFormat::XHTML);
    setText(text);
}
