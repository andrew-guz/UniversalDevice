#include "ThermometerValuesController.hpp"

#include <chrono>
#include <cstddef>
#include <filesystem>
#include <tuple>
#include <vector>

#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "CommandsController.hpp"
#include "Device.hpp"
#include "DevicesController.hpp"
#include "Enums.hpp"
#include "SettingsController.hpp"
#include "Storage.hpp"
#include "ThermometerValue.hpp"
#include "Uuid.hpp"

namespace {
    bool operator==(const ThermometerValue& first, const ThermometerValue& second) {
        return std::tie(first._value, first._timestamp) == std::tie(second._value, second._timestamp);
    }
} // namespace

TEST_CASE("ThermometerValuesController") {
    std::filesystem::path dbPath = std::filesystem::temp_directory_path() / "test.db";
    if (std::filesystem::exists(dbPath))
        std::filesystem::remove(dbPath);

    try {
        Storage storage{ dbPath };

        SettingsController settingsController{ &storage };
        CommandsController commandsController{ &storage };
        DevicesController devicesController{ &storage, settingsController, commandsController };
        ThermometerValuesController thermometerValuesController{ &storage, devicesController };

        Uuid deviceId;

        devicesController.Add(Device{
            ._id = deviceId,
            ._type = DeviceType::Thermometer,
        });

        std::chrono::system_clock::time_point timePoint1 = std::chrono::system_clock::now();
        std::chrono::system_clock::time_point timePoint2 = timePoint1 + std::chrono::seconds{ 1 };

        ThermometerValue value1{
            ._value = 10.0f,
            ._timestamp = timePoint1,
        };
        ThermometerValue value2{
            ._value = 20.0f,
            ._timestamp = timePoint2,
        };

        REQUIRE(thermometerValuesController.Add(deviceId, value1));

        REQUIRE(thermometerValuesController.Get(Uuid{}).empty());
        REQUIRE(thermometerValuesController.Get(Uuid{}, 10).empty());

        REQUIRE(thermometerValuesController.Get(deviceId).size() == 1);
        REQUIRE(thermometerValuesController.Get(deviceId, 100).size() == 1);
        REQUIRE((thermometerValuesController.Get(deviceId).at(0) == value1));

        REQUIRE(thermometerValuesController.Add(deviceId, value2));
        REQUIRE(thermometerValuesController.Get(deviceId).size() == 1);
        REQUIRE(thermometerValuesController.Get(deviceId, 100).size() == 2);
        REQUIRE((thermometerValuesController.Get(deviceId).at(0) == value2));
        REQUIRE((thermometerValuesController.Get(deviceId, 100).at(0) == value2));
        REQUIRE((thermometerValuesController.Get(deviceId, 100).at(1) == value1));

        REQUIRE(thermometerValuesController.Add(deviceId,
                                                ThermometerValue{
                                                    ._value = ThermometerValue::InvalidTemperature,
                                                }) == false);
    } catch (...) {
        REQUIRE(false);
    }

    std::filesystem::remove(dbPath);
}

TEST_CASE("ThermometerValuesController multiple add") {
    std::filesystem::path dbPath = std::filesystem::temp_directory_path() / "test.db";
    if (std::filesystem::exists(dbPath))
        std::filesystem::remove(dbPath);

    try {
        Storage storage{ dbPath };

        SettingsController settingsController{ &storage };
        CommandsController commandsController{ &storage };
        DevicesController devicesController{ &storage, settingsController, commandsController };
        ThermometerValuesController thermometerValuesController{ &storage, devicesController };

        Uuid deviceId;

        devicesController.Add(Device{
            ._id = deviceId,
            ._type = DeviceType::Thermometer,
        });

        std::chrono::system_clock::time_point timePoint = std::chrono::system_clock::now();

        constexpr std::size_t count = 100;
        for (std::size_t i = 0; i < count; ++i) {

            REQUIRE(thermometerValuesController.Add(deviceId,
                                                    ThermometerValue{
                                                        ._value = 10.0f,
                                                        ._timestamp = timePoint - std::chrono::seconds{ i },
                                                    }));
        };

        REQUIRE(thermometerValuesController.Get(deviceId).size() == 1);
        REQUIRE(thermometerValuesController.Get(deviceId, count / 2).size() == count / 2);
        REQUIRE(thermometerValuesController.Get(deviceId, count).size() == count);
    } catch (...) {
        REQUIRE(false);
    }

    std::filesystem::remove(dbPath);
}
