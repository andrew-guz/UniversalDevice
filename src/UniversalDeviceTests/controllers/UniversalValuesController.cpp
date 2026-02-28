#include "UniversalValuesController.hpp"

#include <chrono>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "Storage.hpp"
#include "UniversalData.hpp"
#include "UniversalValue.hpp"
#include "Uuid.hpp"

namespace {
    bool operator==(const UniversalValue& first, const UniversalValue& second) {
        return std::tie(first._values, first._timestamp) == std::tie(second._values, second._timestamp);
    }
} // namespace

TEST_CASE("UniversalValuesController") {
    std::filesystem::path dbPath = std::filesystem::temp_directory_path() / "test.db";
    if (std::filesystem::exists(dbPath))
        std::filesystem::remove(dbPath);

    try {
        Storage storage{ dbPath };

        UniversalValuesController universalValuesController{ &storage };

        Uuid deviceId;

        std::chrono::system_clock::time_point timePoint1 = std::chrono::system_clock::now();
        std::chrono::system_clock::time_point timePoint2 = timePoint1 + std::chrono::seconds{ 1 };

        UniversalValue value1{
            ._values =
                std::map<std::string, UniversalData>{
                    { "a", true },
                },
            ._timestamp = timePoint1,
        };
        UniversalValue value2{
            ._values =
                std::map<std::string, UniversalData>{
                    { "b", 5.0f },
                },
            ._timestamp = timePoint2,
        };

        REQUIRE(universalValuesController.Add(deviceId, value1));

        REQUIRE(universalValuesController.Get(Uuid{}).empty());
        REQUIRE(universalValuesController.Get(Uuid{}, 10).empty());

        REQUIRE(universalValuesController.Get(deviceId).size() == 1);
        REQUIRE(universalValuesController.Get(deviceId, 100).size() == 1);
        REQUIRE((universalValuesController.Get(deviceId).at(0) == value1));

        REQUIRE(universalValuesController.Add(deviceId, value2));
        REQUIRE(universalValuesController.Get(deviceId).size() == 1);
        REQUIRE(universalValuesController.Get(deviceId, 100).size() == 2);
        REQUIRE((universalValuesController.Get(deviceId).at(0) == value2));
        REQUIRE((universalValuesController.Get(deviceId, 100).at(0) == value2));
        REQUIRE((universalValuesController.Get(deviceId, 100).at(1) == value1));
    } catch (...) {
        REQUIRE(false);
    }

    std::filesystem::remove(dbPath);
}
