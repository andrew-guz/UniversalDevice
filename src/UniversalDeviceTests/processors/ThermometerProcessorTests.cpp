#include <chrono>
#include <filesystem>

#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include "CommandsController.hpp"
#include "DevicesController.hpp"
#include "Enums.hpp"
#include "Marshaling.hpp"
#include "Message.hpp"
#include "MessageHelper.hpp"
#include "SettingsController.hpp"
#include "Storage.hpp"
#include "ThermometerProcessor.hpp"
#include "ThermometerValue.hpp"
#include "ThermometerValuesController.hpp"
#include "Uuid.hpp"

TEST_CASE("ThermometerProcessor acknowledge") {
    std::filesystem::path dbPath = std::filesystem::temp_directory_path() / "thermometer_processor_test.db";
    if (std::filesystem::exists(dbPath))
        std::filesystem::remove(dbPath);

    try {
        Storage storage{ dbPath };

        SettingsController settingsController{ &storage };
        CommandsController commandsController{ &storage };
        DevicesController devicesController{ &storage, settingsController, commandsController };
        ThermometerValuesController thermometerValuesController{ &storage, devicesController };
        ThermometerProcessor processor{ thermometerValuesController };

        const Uuid deviceId;

        SECTION("valid temperature is stored and acknowledged") {
            const Message message = MessageHelper::CreateDeviceMessage(
                DeviceType::Thermometer, deviceId, Subject::ThermometerCurrentValue, ThermometerValue{ ._value = 21.5f });

            const nlohmann::json result = processor.ProcessMessage(std::chrono::system_clock::now(), message);

            REQUIRE(result.contains("acknowledge"));
            REQUIRE(result.at("acknowledge").get<Uuid>() == message._header._id);
            REQUIRE(thermometerValuesController.Get(deviceId).size() == 1);
        }

        SECTION("-127 (no sensor) is not stored, but acknowledged") {
            const Message message = MessageHelper::CreateDeviceMessage(DeviceType::Thermometer,
                                                                       deviceId,
                                                                       Subject::ThermometerCurrentValue,
                                                                       ThermometerValue{ ._value = ThermometerValue::InvalidTemperature });

            const nlohmann::json result = processor.ProcessMessage(std::chrono::system_clock::now(), message);

            REQUIRE(result.contains("acknowledge"));
            REQUIRE(result.at("acknowledge").get<Uuid>() == message._header._id);
            REQUIRE(thermometerValuesController.Get(deviceId).empty());
        }
    } catch (...) {
        REQUIRE(false);
    }

    std::filesystem::remove(dbPath);
}
