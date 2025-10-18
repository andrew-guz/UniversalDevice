#pragma once

#include <chrono>
#include <vector>

#include "Cache.hpp"
#include "CommandsController.hpp"
#include "Controller.hpp"
#include "Device.hpp"
#include "IQueryExecutor.hpp"
#include "SettingsController.hpp"
#include "Uuid.hpp"

class DevicesController final : public Controller {
public:
    DevicesController(IQueryExecutor* queryExecutor, SettingsController& settingsController, CommandsController& commandsController);

    ~DevicesController() = default;

    bool Add(const Device& device);

    std::vector<Device> List() const;

    std::optional<Device> Get(const Uuid& id) const;

    bool UpdateDeviceTimestamp(const Uuid& id, std::chrono::system_clock::time_point timestamp);

    bool UpdateDeviceName(const Uuid& id, const std::string& name);

    bool UpdateDeviceGroup(const Uuid& id, const std::string& group);

    bool Remove(const Uuid& id);

private:
    void FillCache() const;

private:
    mutable Cache<Uuid, Device> _cache;
    SettingsController& _settingsController;
    CommandsController& _commandsController;
};
