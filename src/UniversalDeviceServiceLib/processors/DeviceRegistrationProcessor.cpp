#include "DeviceRegistrationProcessor.hpp"

#include <fmt/format.h>

#include "Device.hpp"
#include "Enums.hpp"
#include "Marshaling.hpp"

DeviceRegistrationProcessor::DeviceRegistrationProcessor(IQueryExecutor* queryExecutor, DevicesController& devicesController) :
    BaseProcessorWithQueryExecutor(queryExecutor),
    _devicesController(devicesController) {}

nlohmann::json DeviceRegistrationProcessor::ProcessMessage(const std::chrono::system_clock::time_point& timestamp, const Message& message) {

    const Uuid id = message._header._description._id;
    if (_devicesController.Get(id).has_value())
        _devicesController.UpdateDeviceTimestamp(id, timestamp);
    else {
        Device device{
            ._id = id,
            ._type = std::get<DeviceType>(message._header._description._type),
            ._timestamp = timestamp,
        };
        _devicesController.Add(device);
    }

    return {};
}
