#include <vector>

#include <catch2/catch_all.hpp>
#include <nlohmann/json_fwd.hpp>

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
        ._commands = {
            { Uuid{}, "command" },
            { Uuid{}, "command2" },
        },
    };

    const nlohmann::json expectedJson{
        { "id", scenario._id }, { "name", "test" }, { "activate", activate }, { "deactivate", deactivate }, { "commands", scenario._commands },
    };

    const nlohmann::json scenarioToJson = static_cast<nlohmann::json>(scenario);
    REQUIRE(scenarioToJson == expectedJson);

    const Scenario scenarioFromJson = expectedJson.get<Scenario>();
    REQUIRE(scenarioFromJson == scenario);

    std::vector<Scenario> scenarios;
    const nlohmann::json scenariosVectorJson1 = static_cast<nlohmann::json>(scenarios);
    REQUIRE(scenariosVectorJson1.dump() == "[]");

    Scenario vectorScenario{
        ._name = "test",
    };
    scenarios.push_back(vectorScenario);
    const nlohmann::json scenariosVectorJson2 = static_cast<nlohmann::json>(scenarios);
    const nlohmann::json expectedScenariosJson = nlohmann::json::array_t{
        static_cast<nlohmann::json>(vectorScenario),
    };
    REQUIRE(scenariosVectorJson2 == expectedScenariosJson);
}
