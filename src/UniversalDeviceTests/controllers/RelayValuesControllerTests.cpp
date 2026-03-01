#include <chrono>
#include <filesystem>
#include <tuple>
#include <vector>

#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "RelayValue.hpp"
#include "RelayValuesController.hpp"
#include "Storage.hpp"
#include "Uuid.hpp"

namespace {
    bool operator==(const RelayValue& first, const RelayValue& second) {
        return std::tie(first._state, first._timestamp) == std::tie(second._state, second._timestamp);
    }
} // namespace

TEST_CASE("RelayValuesControllerTests") {
    std::filesystem::path dbPath = std::filesystem::temp_directory_path() / "test.db";
    if (std::filesystem::exists(dbPath))
        std::filesystem::remove(dbPath);

    try {
        Storage storage{ dbPath };

        RelayValuesController relayValuesController{ &storage };

        Uuid deviceId;

        std::chrono::system_clock::time_point timePoint1 = std::chrono::system_clock::now();
        std::chrono::system_clock::time_point timePoint2 = timePoint1 + std::chrono::seconds{ 1 };

        RelayValue value1{
            ._state = 1,
            ._timestamp = timePoint1,
        };
        RelayValue value2{
            ._state = 1,
            ._timestamp = timePoint2,
        };

        REQUIRE(relayValuesController.Add(deviceId, value1));

        REQUIRE(relayValuesController.Get(Uuid{}).empty());
        REQUIRE(relayValuesController.Get(Uuid{}, 10).empty());

        REQUIRE(relayValuesController.Get(deviceId).size() == 1);
        REQUIRE(relayValuesController.Get(deviceId, 100).size() == 1);
        REQUIRE((relayValuesController.Get(deviceId).at(0) == value1));

        REQUIRE(relayValuesController.Add(deviceId, value2));
        REQUIRE(relayValuesController.Get(deviceId).size() == 1);
        REQUIRE(relayValuesController.Get(deviceId, 100).size() == 2);
        REQUIRE((relayValuesController.Get(deviceId).at(0) == value2));
        REQUIRE((relayValuesController.Get(deviceId, 100).at(0) == value2));
        REQUIRE((relayValuesController.Get(deviceId, 100).at(1) == value1));
    } catch (...) {
        REQUIRE(false);
    }

    std::filesystem::remove(dbPath);
}
