#include <optional>

#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json_fwd.hpp>

#include "Marshaling.hpp"

TEST_CASE("OptionalMarshaling") {
    {
        const std::optional<int> value = std::nullopt;

        REQUIRE(static_cast<nlohmann::json>(value).is_null());
        REQUIRE(static_cast<nlohmann::json>(value) == nlohmann::json{});
    }

    {
        const std::optional<int> value = nlohmann::json{}.get<std::optional<int>>();
        REQUIRE(!value.has_value());
    }

    {
        const std::optional<int> value = 56;
        const nlohmann::json json = 56;
        REQUIRE(!static_cast<nlohmann::json>(value).is_null());
        REQUIRE(static_cast<nlohmann::json>(value) == json);
    }

    {
        const nlohmann::json json = 65;
        const std::optional<int> value = json.get<std::optional<int>>();
        REQUIRE(value.has_value());
        REQUIRE(value.value() == 65);
    }
}
