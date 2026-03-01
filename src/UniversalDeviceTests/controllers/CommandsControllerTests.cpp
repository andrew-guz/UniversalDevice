#include <filesystem>
#include <optional>
#include <variant>

#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "Command.hpp"
#include "CommandsController.hpp"
#include "RelayState.hpp"
#include "Storage.hpp"
#include "ThermometerLedBrightness.hpp"
#include "Uuid.hpp"

TEST_CASE("CommandsController") {
    std::filesystem::path dbPath = std::filesystem::temp_directory_path() / "test.db";
    if (std::filesystem::exists(dbPath))
        std::filesystem::remove(dbPath);

    try {
        Storage storage{ dbPath };

        CommandsController commandsController{ &storage };

        REQUIRE(commandsController.Get(Uuid{}).has_value() == false);

        Uuid id;
        RelayState relayState{
            ._state = 1,
        };

        REQUIRE(commandsController.AddOrUpdate(id, relayState) == true);

        const auto checkRelayState = [&id, &relayState](CommandsController& controller) {
            const std::optional<Command> getResult = controller.Get(id);
            REQUIRE(getResult.has_value() == true);
            REQUIRE(std::holds_alternative<RelayState>(getResult.value()) == true);
            REQUIRE(std::get<RelayState>(getResult.value())._state == relayState._state);
        };

        checkRelayState(commandsController);
        {
            CommandsController commandsController{ &storage };
            checkRelayState(commandsController);
        }

        ThermometerLedBrightness thermometerLedBrightness{
            ._brightness = 4,
        };

        REQUIRE(commandsController.AddOrUpdate(id, thermometerLedBrightness) == true);

        const auto checkThermometerLedBrightness = [&id, &thermometerLedBrightness](CommandsController& controller) {
            const std::optional<Command> getResult = controller.Get(id);
            REQUIRE(getResult.has_value() == true);
            REQUIRE(std::holds_alternative<ThermometerLedBrightness>(getResult.value()) == true);
            REQUIRE(std::get<ThermometerLedBrightness>(getResult.value())._brightness == thermometerLedBrightness._brightness);
        };

        checkThermometerLedBrightness(commandsController);
        {
            CommandsController commandsController{ &storage };
            checkThermometerLedBrightness(commandsController);
        }

        REQUIRE(commandsController.Remove(id) == true);

        const std::optional<Command> getResult = commandsController.Get(id);
        REQUIRE(getResult.has_value() == false);

        REQUIRE(commandsController.Remove(Uuid{}) == false);
    } catch (...) {
        REQUIRE(false);
    }

    std::filesystem::remove(dbPath);
}
