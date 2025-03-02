#include <optional>

#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json_fwd.hpp>

#include "Marshaling.hpp"
#include "UniversalData.hpp"
#include "UniversalDeviceCurrentValues.hpp"

TEST_CASE("UniversalDeviceCurrentValuesJson") {
    const UniversalDeviceCurrentValues universalDeviceCurrentValues {
        ._values = {
            { "bool_param", true },
            { "int_param", 42 },
            { "double_param", 3.14 },
            { "string_param", std::string {"test"} },
        },
    };
    const nlohmann::json expectedJson{
        { "values",
          nlohmann::json{
              {
                  "bool_param",
                  {
                      { "type", "boolean" },
                      { "value", true },
                  },
              },
              {
                  "int_param",
                  {
                      { "type", "integer" },
                      { "value", 42 },
                  },
              },
              {
                  "double_param",
                  {
                      { "type", "double" },
                      { "value", 3.14 },
                  },
              },
              {
                  "string_param",
                  {
                      { "type", "string" },
                      { "value", "test" },
                  },
              },
          } },
    };

    const nlohmann::json universalDeviceCurrentValuesToJson = static_cast<nlohmann::json>(universalDeviceCurrentValues);
    REQUIRE(universalDeviceCurrentValuesToJson == expectedJson);

    const UniversalDeviceCurrentValues universalDeviceCurrentValuesFromJson = expectedJson.get<UniversalDeviceCurrentValues>();
    REQUIRE(universalDeviceCurrentValuesFromJson._values.size() == universalDeviceCurrentValues._values.size());
    for (const std::pair<std::string, UniversalData>& pair : universalDeviceCurrentValues._values) {
        REQUIRE(universalDeviceCurrentValuesFromJson._values.count(pair.first) != 0);
        const bool parametersAreEqual = universalDeviceCurrentValuesFromJson._values.at(pair.first) == pair.second;
        REQUIRE(parametersAreEqual);
    }
}
