#include <chrono>
#include <cstddef>
#include <filesystem>

#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "CleanupController.hpp"
#include "CommandsController.hpp"
#include "DevicesController.hpp"
#include "EventsController.hpp"
#include "MotionRelayValue.hpp"
#include "MotionRelayValuesController.hpp"
#include "RelayValue.hpp"
#include "RelayValuesController.hpp"
#include "ScenariosController.hpp"
#include "SettingsController.hpp"
#include "Storage.hpp"
#include "ThermometerValue.hpp"
#include "ThermometerValuesController.hpp"
#include "UniversalValue.hpp"
#include "UniversalValuesController.hpp"
#include "Uuid.hpp"
#include <bits/chrono.h>

TEST_CASE("CleanupController") {
    std::filesystem::path dbPath = std::filesystem::temp_directory_path() / "test.db";
    if (std::filesystem::exists(dbPath))
        std::filesystem::remove(dbPath);

    try {
        const Uuid id{};
        Storage storage{ dbPath };
        SettingsController settingsController{ &storage };
        CommandsController commandsController{ &storage };
        EventsController eventsController{ &storage };
        ScenariosController scenariosController{ &storage, eventsController, commandsController };
        DevicesController devicesController{ &storage, settingsController, commandsController, scenariosController };
        {
            ThermometerValuesController thermometerValuesController{ &storage, devicesController };
            RelayValuesController relayValuesController{ &storage };
            MotionRelayValuesController motionRelayValuesController{ &storage };
            UniversalValuesController universalValuesController{ &storage };
            CleanupController cleanupController{
                &storage, thermometerValuesController, relayValuesController, motionRelayValuesController, universalValuesController
            };

            const std::chrono::system_clock::time_point timePoint = std::chrono::system_clock::now();
            const std::chrono::system_clock::time_point future = timePoint + std::chrono::weeks{ 3 };
            REQUIRE(thermometerValuesController.Add(id, ThermometerValue{ ._value = 10.0f, ._timestamp = timePoint }) == true);
            REQUIRE(relayValuesController.Add(id, RelayValue{ ._state = true, ._timestamp = timePoint }) == true);
            REQUIRE(motionRelayValuesController.Add(id, MotionRelayValue{ ._motion = true, ._state = true, ._timestamp = timePoint }) == true);
            REQUIRE(universalValuesController.Add(id, UniversalValue{ ._values = {}, ._timestamp = timePoint }) == true);

            REQUIRE(thermometerValuesController.Get(id).size() == 1);
            REQUIRE(relayValuesController.Get(id).size() == 1);
            REQUIRE(motionRelayValuesController.Get(id).size() == 1);
            REQUIRE(universalValuesController.Get(id).size() == 1);

            cleanupController.Cleanup(future);

            // cache was cleared
            REQUIRE(thermometerValuesController.Get(id).size() == 0);
            REQUIRE(relayValuesController.Get(id).size() == 0);
            REQUIRE(motionRelayValuesController.Get(id).size() == 0);
            REQUIRE(universalValuesController.Get(id).size() == 0);
        }
        {
            Storage storage{ dbPath };

            SettingsController settingsController{ &storage };
            CommandsController commandsController{ &storage };
            EventsController eventsController{ &storage };
            ScenariosController scenariosController{ &storage, eventsController, commandsController };
            DevicesController devicesController{ &storage, settingsController, commandsController, scenariosController };
            ThermometerValuesController thermometerValuesController{ &storage, devicesController };
            RelayValuesController relayValuesController{ &storage };
            MotionRelayValuesController motionRelayValuesController{ &storage };
            UniversalValuesController universalValuesController{ &storage };

            // real database was cleared
            REQUIRE(thermometerValuesController.Get(id).size() == 0);
            REQUIRE(relayValuesController.Get(id).size() == 0);
            REQUIRE(motionRelayValuesController.Get(id).size() == 0);
            REQUIRE(universalValuesController.Get(id).size() == 0);
        }
    } catch (...) {
        REQUIRE(false);
    }

    std::filesystem::remove(dbPath);
}

TEST_CASE("CleanupController only several") {
    std::filesystem::path dbPath = std::filesystem::temp_directory_path() / "test.db";
    if (std::filesystem::exists(dbPath))
        std::filesystem::remove(dbPath);

    try {
        const Uuid id{};
        Storage storage{ dbPath };
        SettingsController settingsController{ &storage };
        CommandsController commandsController{ &storage };
        EventsController eventsController{ &storage };
        ScenariosController scenariosController{ &storage, eventsController, commandsController };
        DevicesController devicesController{ &storage, settingsController, commandsController, scenariosController };
        constexpr std::size_t count = 50;
        constexpr std::size_t requestCount = count * 2;
        {
            ThermometerValuesController thermometerValuesController{ &storage, devicesController };
            RelayValuesController relayValuesController{ &storage };
            MotionRelayValuesController motionRelayValuesController{ &storage };
            UniversalValuesController universalValuesController{ &storage };
            CleanupController cleanupController{
                &storage, thermometerValuesController, relayValuesController, motionRelayValuesController, universalValuesController
            };

            const std::chrono::system_clock::time_point timePoint = std::chrono::system_clock::now();

            for (std::size_t i = 0; i < count; ++i) {
                const std::chrono::system_clock::time_point time1 = timePoint - std::chrono::seconds{ i + 1 };
                const std::chrono::system_clock::time_point time2 = time1 - std::chrono::weeks{ 2 };
                REQUIRE(thermometerValuesController.Add(id, ThermometerValue{ ._value = 10.0f, ._timestamp = time1 }) == true);
                REQUIRE(thermometerValuesController.Add(id, ThermometerValue{ ._value = 10.0f, ._timestamp = time2 }) == true);
                REQUIRE(relayValuesController.Add(id, RelayValue{ ._state = true, ._timestamp = time1 }) == true);
                REQUIRE(relayValuesController.Add(id, RelayValue{ ._state = true, ._timestamp = time2 }) == true);
                REQUIRE(motionRelayValuesController.Add(id, MotionRelayValue{ ._motion = true, ._state = true, ._timestamp = time1 }) == true);
                REQUIRE(motionRelayValuesController.Add(id, MotionRelayValue{ ._motion = true, ._state = true, ._timestamp = time2 }) == true);
                REQUIRE(universalValuesController.Add(id, UniversalValue{ ._values = {}, ._timestamp = time1 }) == true);
                REQUIRE(universalValuesController.Add(id, UniversalValue{ ._values = {}, ._timestamp = time2 }) == true);
            }

            cleanupController.Cleanup(timePoint);

            // cache was partially cleaned
            REQUIRE(thermometerValuesController.Get(id, requestCount).size() == count);
            REQUIRE(relayValuesController.Get(id, requestCount).size() == count);
            REQUIRE(motionRelayValuesController.Get(id, requestCount).size() == count);
            REQUIRE(universalValuesController.Get(id, requestCount).size() == count);
        }
        {
            Storage storage{ dbPath };

            SettingsController settingsController{ &storage };
            CommandsController commandsController{ &storage };
            EventsController eventsController{ &storage };
            ScenariosController scenariosController{ &storage, eventsController, commandsController };
            DevicesController devicesController{ &storage, settingsController, commandsController, scenariosController };
            ThermometerValuesController thermometerValuesController{ &storage, devicesController };
            RelayValuesController relayValuesController{ &storage };
            MotionRelayValuesController motionRelayValuesController{ &storage };
            UniversalValuesController universalValuesController{ &storage };

            // real database was partially cleaned
            REQUIRE(thermometerValuesController.Get(id, requestCount).size() == count);
            REQUIRE(relayValuesController.Get(id, requestCount).size() == count);
            REQUIRE(motionRelayValuesController.Get(id, requestCount).size() == count);
            REQUIRE(universalValuesController.Get(id, requestCount).size() == count);
        }
    } catch (...) {
        REQUIRE(false);
    }

    std::filesystem::remove(dbPath);
}
