#include <chrono>
#include <filesystem>
#include <tuple>
#include <vector>

#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "MotionRelayValue.hpp"
#include "MotionRelayValuesController.hpp"
#include "Storage.hpp"
#include "Uuid.hpp"

namespace {
    bool operator==(const MotionRelayValue& first, const MotionRelayValue& second) {
        return std::tie(first._motion, first._state, first._timestamp) == std::tie(second._motion, second._state, second._timestamp);
    }
} // namespace

TEST_CASE("MotionRelayValuesControllerTests") {
    std::filesystem::path dbPath = std::filesystem::temp_directory_path() / "test.db";
    if (std::filesystem::exists(dbPath))
        std::filesystem::remove(dbPath);

    try {
        Storage storage{ dbPath };

        MotionRelayValuesController motionRelayValuesController{ &storage };

        Uuid deviceId;

        std::chrono::system_clock::time_point timePoint1 = std::chrono::system_clock::now();
        std::chrono::system_clock::time_point timePoint2 = timePoint1 + std::chrono::seconds{ 1 };

        MotionRelayValue value1{
            ._motion = true,
            ._state = 1,
            ._timestamp = timePoint1,
        };
        MotionRelayValue value2{
            ._motion = true,
            ._state = 1,
            ._timestamp = timePoint2,
        };

        REQUIRE(motionRelayValuesController.Add(deviceId, value1));

        REQUIRE(motionRelayValuesController.Get(Uuid{}).empty());
        REQUIRE(motionRelayValuesController.Get(Uuid{}, 10).empty());

        REQUIRE(motionRelayValuesController.Get(deviceId).size() == 1);
        REQUIRE(motionRelayValuesController.Get(deviceId, 100).size() == 1);
        REQUIRE((motionRelayValuesController.Get(deviceId).at(0) == value1));

        REQUIRE(motionRelayValuesController.Add(deviceId, value2));
        REQUIRE(motionRelayValuesController.Get(deviceId).size() == 1);
        REQUIRE(motionRelayValuesController.Get(deviceId, 100).size() == 2);
        REQUIRE((motionRelayValuesController.Get(deviceId).at(0) == value2));
        REQUIRE((motionRelayValuesController.Get(deviceId, 100).at(0) == value2));
        REQUIRE((motionRelayValuesController.Get(deviceId, 100).at(1) == value1));
    } catch (...) {
        REQUIRE(false);
    }

    std::filesystem::remove(dbPath);
}
