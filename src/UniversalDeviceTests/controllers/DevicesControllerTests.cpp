
#include <chrono>
#include <filesystem>
#include <optional>
#include <string>
#include <thread>
#include <utility>

#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "CommandsController.hpp"
#include "Device.hpp"
#include "DeviceDescription.hpp"
#include "DevicesController.hpp"
#include "Enums.hpp"
#include "Event.hpp"
#include "EventUtils.hpp"
#include "EventsController.hpp"
#include "PeriodSettings.hpp"
#include "RelayState.hpp"
#include "Scenario.hpp"
#include "ScenariosController.hpp"
#include "SettingsController.hpp"
#include "Storage.hpp"
#include "ThermometerEvent.hpp"
#include "ThermometerLedBrightness.hpp"
#include "Uuid.hpp"

TEST_CASE("DevicesController") {
    std::filesystem::path dbPath = std::filesystem::temp_directory_path() / "test.db";
    if (std::filesystem::exists(dbPath))
        std::filesystem::remove(dbPath);

    try {
        Storage storage{ dbPath };

        SettingsController settingsController{ &storage };
        CommandsController commandsController{ &storage };
        EventsController eventsController{ &storage };
        ScenariosController scenariosController{ &storage, eventsController, commandsController };
        DevicesController devicesController{ &storage, settingsController, commandsController, scenariosController };

        REQUIRE(devicesController.List().size() == 0);
        REQUIRE(devicesController.Get(Uuid{}).has_value() == false);

        Device device{
            ._type = DeviceType::Thermometer,
            ._timestamp = std::chrono::system_clock::now(),
        };

        REQUIRE(devicesController.Add(device) == true);

        REQUIRE(devicesController.List().size() == 1);
        REQUIRE(devicesController.Get(device._id).has_value() == true);

        {
            REQUIRE(devicesController.UpdateDeviceTimestamp(Uuid{}, std::chrono::system_clock::now()).has_value() == false);

            const std::chrono::system_clock::time_point currentTimestamp = device._timestamp;
            std::this_thread::sleep_for(std::chrono::seconds{ 1 });
            const std::chrono::system_clock::time_point newTimestamp = std::chrono::system_clock::now();
            std::optional<Device> updatedDevice = devicesController.UpdateDeviceTimestamp(device._id, newTimestamp);
            REQUIRE(updatedDevice.has_value() == true);
            device = updatedDevice.value();
            REQUIRE(device._timestamp != currentTimestamp);
            REQUIRE(device._timestamp == newTimestamp);
        }

        {
            REQUIRE(devicesController.UpdateDeviceName(Uuid{}, std::string{}).has_value() == false);

            const std::string currentName = device._name;
            const std::string newName = "New name";
            std::optional<Device> updatedDevice = devicesController.UpdateDeviceName(device._id, newName);
            REQUIRE(updatedDevice.has_value() == true);
            device = updatedDevice.value();
            REQUIRE(device._name != currentName);
            REQUIRE(device._name == newName);
        }

        {
            REQUIRE(devicesController.UpdateDeviceGroup(Uuid{}, std::string{}).has_value() == false);

            const std::string currentGroup = device._group;
            const std::string newGroup = "New group";
            std::optional<Device> updatedDevice = devicesController.UpdateDeviceGroup(device._id, newGroup);
            REQUIRE(updatedDevice.has_value() == true);
            device = updatedDevice.value();
            REQUIRE(device._group != currentGroup);
            REQUIRE(device._group == newGroup);
        }

        commandsController.AddOrUpdate(device._id,
                                       ThermometerLedBrightness{
                                           ._brightness = 3,
                                       });
        settingsController.AddOrUpdate(device._id,
                                       PeriodSettings{
                                           ._period = 5,
                                       });

        REQUIRE(commandsController.Get(device._id).has_value() == true);
        REQUIRE(settingsController.Get(device._id).has_value() == true);

        REQUIRE(devicesController.Remove(Uuid{}) == false);

        REQUIRE(devicesController.Remove(device._id) == true);

        REQUIRE(devicesController.List().size() == 0);
        REQUIRE(devicesController.Get(device._id).has_value() == false);

        REQUIRE(commandsController.Get(device._id).has_value() == false);
        REQUIRE(settingsController.Get(device._id).has_value() == false);
    } catch (...) {
        REQUIRE(false);
    }

    std::filesystem::remove(dbPath);
}

TEST_CASE("DevicesController remove from other tables") {
    std::filesystem::path dbPath = std::filesystem::temp_directory_path() / "test.db";
    if (std::filesystem::exists(dbPath))
        std::filesystem::remove(dbPath);

    try {
        const Uuid device1Id{};
        const Uuid device2Id{};
        const Uuid device3Id{};
        const Uuid eventId{};
        const Uuid scenarioId{};

        {
            Storage storage{ dbPath };

            SettingsController settingsController{ &storage };
            CommandsController commandsController{ &storage };
            EventsController eventsController{ &storage };
            ScenariosController scenariosController{ &storage, eventsController, commandsController };
            DevicesController devicesController{ &storage, settingsController, commandsController, scenariosController };

            Device device1{
                ._id = device1Id,
                ._type = DeviceType::Thermometer,
                ._timestamp = std::chrono::system_clock::now(),
            };
            Device device2{
                ._id = device2Id,
                ._type = DeviceType::Relay,
                ._timestamp = std::chrono::system_clock::now(),
            };
            Device device3{
                ._id = device3Id,
                ._type = DeviceType::Relay,
                ._timestamp = std::chrono::system_clock::now(),
            };

            REQUIRE(devicesController.Add(device1) == true);
            REQUIRE(devicesController.Add(device2) == true);
            REQUIRE(devicesController.Add(device3) == true);

            ThermometerEvent thermometerEvent;
            thermometerEvent._id = eventId;
            thermometerEvent._provider = DeviceDescription{
                ._type = DeviceType::Thermometer,
                ._id = device1Id,
            };
            thermometerEvent._receiver = DeviceDescription{
                ._type = DeviceType::Relay,
                ._id = device2Id,
            };
            Event event = std::move(thermometerEvent);
            REQUIRE(eventsController.Add(event) == true);

            Scenario scenario{
                ._id = scenarioId,
                ._activateEvent = { eventId, },
                ._commands = { { device3Id, RelayState{ ._state = true }} },
            };
            REQUIRE(scenariosController.Add(scenario));

            REQUIRE(devicesController.Remove(device3Id) == true);

            REQUIRE(scenariosController.Get(scenarioId)->_commands.size() == 0);
        }
        {
            Storage storage{ dbPath };

            SettingsController settingsController{ &storage };
            CommandsController commandsController{ &storage };
            EventsController eventsController{ &storage };
            ScenariosController scenariosController{ &storage, eventsController, commandsController };
            DevicesController devicesController{ &storage, settingsController, commandsController, scenariosController };

            REQUIRE(scenariosController.Get(scenarioId)->_commands.size() == 0);
        }
    } catch (...) {
        REQUIRE(false);
    }

    std::filesystem::remove(dbPath);
}
