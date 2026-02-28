#include "DeviceRegistrationProcessor.hpp"

#include <chrono>

#include <fmt/format.h>
#include <nlohmann/json_fwd.hpp>

#include "Device.hpp"
#include "DeviceDescription.hpp"
#include "DevicesController.hpp"
#include "Marshaling.hpp"
#include "Provider.hpp"

DeviceRegistrationProcessor::DeviceRegistrationProcessor(DevicesController& devicesController) :
    _devicesController(devicesController) {}

nlohmann::json DeviceRegistrationProcessor::ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) {
    const Uuid id = GetProviderId(message._header._description);
    if (_devicesController.Get(id).has_value())
        _devicesController.UpdateDeviceTimestamp(id, timestamp);
    else {
        Device device{
            ._id = id,
            ._type = std::get<DeviceDescription>(message._header._description)._type,
            ._timestamp = timestamp,
        };
        _devicesController.Add(device);
    }

    return {};
}
