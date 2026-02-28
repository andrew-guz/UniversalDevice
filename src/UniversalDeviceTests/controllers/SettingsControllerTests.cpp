#include <filesystem>
#include <optional>
#include <variant>

#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "MotionRelaySettings.hpp"
#include "PeriodSettings.hpp"
#include "Settings.hpp"
#include "SettingsController.hpp"
#include "Storage.hpp"
#include "Uuid.hpp"

TEST_CASE("SettingsController") {
    std::filesystem::path dbPath = std::filesystem::temp_directory_path() / "test.db";
    if (std::filesystem::exists(dbPath))
        std::filesystem::remove(dbPath);

    try {
        Storage storage{ dbPath };

        SettingsController settingsController{ &storage };

        REQUIRE(settingsController.Get(Uuid{}).has_value() == false);

        Uuid id;
        PeriodSettings periodSettings{
            ._period = 10,
        };

        REQUIRE(settingsController.AddOrUpdate(id, periodSettings) == true);

        const auto checkPeriodSettings = [&id, &periodSettings](SettingsController& controller) {
            const std::optional<Settings> getResult = controller.Get(id);
            REQUIRE(getResult.has_value() == true);
            REQUIRE(std::holds_alternative<PeriodSettings>(getResult.value()) == true);
            REQUIRE(std::get<PeriodSettings>(getResult.value())._period == periodSettings._period);
        };

        checkPeriodSettings(settingsController);
        {
            SettingsController settingsController{ &storage };
            checkPeriodSettings(settingsController);
        }

        MotionRelaySettings motionRelaySettings{
            ._activityTime = 15,
        };

        REQUIRE(settingsController.AddOrUpdate(id, motionRelaySettings) == true);

        const auto checkMotionRelaySettings = [&id, &motionRelaySettings](SettingsController& controller) {
            const std::optional<Settings> getResult = controller.Get(id);
            REQUIRE(getResult.has_value() == true);
            REQUIRE(std::holds_alternative<MotionRelaySettings>(getResult.value()) == true);
            REQUIRE(std::get<MotionRelaySettings>(getResult.value())._activityTime == motionRelaySettings._activityTime);
        };

        checkMotionRelaySettings(settingsController);
        {
            SettingsController settingsController{ &storage };
            checkMotionRelaySettings(settingsController);
        }

        REQUIRE(settingsController.Remove(id) == true);

        const std::optional<Settings> getResult = settingsController.Get(id);
        REQUIRE(getResult.has_value() == false);

        REQUIRE(settingsController.Remove(Uuid{}) == false);
    } catch (...) {
        REQUIRE(false);
    }

    std::filesystem::remove(dbPath);
}
