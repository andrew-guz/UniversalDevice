#include "DevicesController.hpp"

#include "CommandsController.hpp"
#include "Controller.hpp"
#include "DbExtension.hpp"
#include "Device.hpp"
#include "IQueryExecutor.hpp"
#include "Marshaling.hpp"
#include "SettingsController.hpp"
#include "TimeHelper.hpp"

DevicesController::DevicesController(IQueryExecutor* queryExecutor, SettingsController& settingsController, CommandsController& commandsController) :
    Controller(queryExecutor),
    _settingsController(settingsController),
    _commandsController(commandsController) //
{
    FillCache();
}

bool DevicesController::Add(const Device& device) {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    if (_cache.Get(device._id).has_value())
        return false;

    const std::string query = fmt::format("INSERT INTO Devices (id, type, timestamp) VALUES ('{}', '{}', {})",
                                          device._id.data(),
                                          EnumToString(device._type),
                                          TimeHelper::TimeToInt(device._timestamp));
    if (_queryExecutor->Execute(query)) {
        _cache.Add(device._id, device);

        return true;
    } else
        LOG_SQL_ERROR(query);

    return false;
}

std::vector<Device> DevicesController::List() const {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    if (_cache.Size())
        return _cache.List();

    FillCache();

    return _cache.List();
}

std::optional<Device> DevicesController::Get(const Uuid& id) const {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    std::optional<Device> cacheResult = _cache.Get(id);
    if (cacheResult.has_value())
        return cacheResult;

    // Maybe not in cache but in database - we should check
    const std::string query = fmt::format("SELECT * FROM Devices WHERE id = '{}'", id.data());
    std::vector<std::vector<std::string>> data;
    if (_queryExecutor->Select(query, data)) {
        if (data.size() == 0)
            LOG_DEBUG_MSG(fmt::format("No device with id {}", id.data()));
        else if (data.size() == 1) {
            Device device = DbExtension::CreateFromDbStrings<Device>(data[0]);
            _cache.Add(device._id, device);
            return device;
        } else
            LOG_ERROR_MSG(fmt::format("Too many devices with id {}", id.data()));
    } else
        LOG_SQL_ERROR(query);

    return std::nullopt;
}

bool DevicesController::UpdateDeviceTimestamp(const Uuid& id, const std::chrono::system_clock::time_point timestamp) {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    const std::string query = fmt::format("UPDATE Devices SET timestamp = {} WHERE id = '{}'", TimeHelper::TimeToInt(timestamp), id.data());
    if (_queryExecutor->Execute(query)) {
        Device device = _cache.Get(id).value();
        device._timestamp = timestamp;
        _cache.AddOrUpdate(id, device);
        return true;
    } else
        LOG_SQL_ERROR(query);

    return false;
}

bool DevicesController::UpdateDeviceName(const Uuid& id, const std::string& name) {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    const std::string query = fmt::format("UPDATE Devices SET name = '{}' WHERE id = '{}'", name, id.data());
    if (_queryExecutor->Execute(query)) {
        Device device = _cache.Get(id).value();
        device._name = name;
        _cache.AddOrUpdate(id, device);
        return true;
    } else
        LOG_SQL_ERROR(query);

    return false;
}

bool DevicesController::UpdateDeviceGroup(const Uuid& id, const std::string& group) {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    const std::string query = fmt::format("UPDATE Devices SET grp = '{}' WHERE id = '{}'", group, id.data());
    if (_queryExecutor->Execute(query)) {
        Device device = _cache.Get(id).value();
        device._group = group;
        _cache.AddOrUpdate(id, device);
        return true;
    } else
        LOG_SQL_ERROR(query);

    return false;
}

bool DevicesController::Remove(const Uuid& id) {
    std::lock_guard<std::mutex> lockGuard{ _mutex };

    const std::string query = fmt::format("DELETE FROM Devices WHERE id='{}'", id.data());
    if (_queryExecutor->Execute(query)) {
        _cache.Remove(id);

        _settingsController.Remove(id);
        _commandsController.Remove(id);

        return true;
    }

    LOG_SQL_ERROR(query);

    return false;
}

void DevicesController::FillCache() const {
    const std::string query = "SELECT * FROM Devices";
    std::vector<std::vector<std::string>> data;
    if (_queryExecutor->Select(query, data)) {
        const std::vector<Device> result = DbExtension::CreateVectorFromDbStrings<Device>(data);
        for (const Device& device : result) {
            _cache.Add(device._id, device);
        }
    } else
        LOG_SQL_ERROR(query);
}
