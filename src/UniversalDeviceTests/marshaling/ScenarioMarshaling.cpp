#include "catch2/catch_test_macros.hpp"
#include "nlohmann/json_fwd.hpp"
#include <catch2/catch_all.hpp>

#include "Marshaling.hpp"
#include "Scenario.hpp"
#include "Uuid.hpp"

TEST_CASE("ScenarioJson") {
    const Uuid id;
    std::set<Uuid> activate{
        Uuid{},
    };
    std::set<Uuid> deactivate{
        Uuid{},
        Uuid{},
    };

    const Scenario scenario{
        ._id = id,
        ._name = "test",
        ._activateEvent = activate,
        ._deactivateEvent = deactivate,
    };

    const nlohmann::json expectedJson{
        { "id", scenario._id },
        { "name", "test" },
        { "activate", activate },
        { "deactivate", deactivate },
    };

    const nlohmann::json scenarioToJson = static_cast<nlohmann::json>(scenario);
    REQUIRE(scenarioToJson == expectedJson);

    const Scenario scenarioFromJson = expectedJson.get<Scenario>();
    REQUIRE(scenarioFromJson == scenario);
}
