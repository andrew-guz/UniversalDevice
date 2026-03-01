#include "MessageHelper.hpp"

#include <utility>

#include <nlohmann/json_fwd.hpp>

#include "DeviceDescription.hpp"
#include "Enums.hpp"
#include "Message.hpp"
#include "Provider.hpp"
#include "Uuid.hpp"

namespace {

    Message Create(Provider provider, const Subject subject, const nlohmann::json& data) {
        Message message;
        message._header._description = std::move(provider);
        message._header._subject = subject;
        message._data = data;
        return message;
    }

} // namespace

Message MessageHelper::CreateDeviceMessage(const DeviceType deviceType, const Uuid& deviceId, const Subject subject, const nlohmann::json& data) {
    return Create(
        DeviceDescription{
            ._type = deviceType,
            ._id = deviceId,
        },
        subject,
        data);
}

Message MessageHelper::CreateClientMessage(const Subject subject, const nlohmann::json& data) { return Create(ClientProvider{}, subject, data); }

Message MessageHelper::CreateEventMessage(const Uuid& eventId, const Subject subject, const nlohmann::json& data) {
    return Create(
        EventProvider{
            ._id = eventId,
        },
        subject,
        data);
}
