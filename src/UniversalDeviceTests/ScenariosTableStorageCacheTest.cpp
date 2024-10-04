#include <filesystem>

#include <catch2/catch_all.hpp>

#include "IStorageCache.hpp"
#include "Scenario.hpp"
#include "SimpleTableStorageCache.hpp"
#include "Storage.hpp"
#include "StorageCacheSharedData.hpp"
#include "Uuid.hpp"

TEST_CASE("ScenariosTableStorageCacheTest") {
    std::filesystem::path dbPath = std::filesystem::temp_directory_path() / "test.db";
    if (std::filesystem::exists(dbPath))
        std::filesystem::remove(dbPath);

    Storage storage{ dbPath };

    IStorageCache* storageCache = GetScenariosCache(&storage);

    Scenario scenario{
        ._name = "test",
    };

    auto checkSelect = [&scenario, &storageCache](bool contains) {
        SimpleTableSelectAllOutput<Scenario> selectAllResult;
        const StorageCacheProblem selectAllProblem = storageCache->SelectAll(selectAllResult);
        REQUIRE(selectAllProblem._type == StorageCacheProblemType::NoProblems);
        if (contains) {
            REQUIRE(selectAllResult._data.size() == 1);
            REQUIRE(selectAllResult._data.at(0) == scenario);

            const SimpleTableSelectInput selectInput{
                ._id = scenario._id,
            };
            SimpleTableSelectOutput<Scenario> selectOutput;
            const StorageCacheProblem selectProblem = storageCache->Select(selectInput, selectOutput);
            REQUIRE(selectProblem._type == StorageCacheProblemType::NoProblems);
            REQUIRE(selectOutput._data == scenario);
        } else {
            REQUIRE(selectAllResult._data.size() == 0);
        }
    };

    checkSelect(false);

    {
        const SimpleTableInsertOrReplaceInput<Scenario> insert{
            ._id = scenario._id,
            ._data = scenario,
        };
        const StorageCacheProblem insertProblem1 = storageCache->InsertOrReplace(insert);
        REQUIRE(insertProblem1._type == StorageCacheProblemType::NoProblems);
    }

    checkSelect(true);

    scenario._activateEvent.emplace(Uuid{});

    {
        const SimpleTableUpdateInput<Scenario> input{
            ._id = scenario._id,
            ._data = scenario,
        };
        const StorageCacheProblem updateProblem = storageCache->Update(input);
        REQUIRE(updateProblem._type == StorageCacheProblemType::NoProblems);
    }

    checkSelect(true);

    {
        const SimpleTableDeleteInput input{
            ._id = scenario._id,
        };
        const StorageCacheProblem removeProblem = storageCache->Delete(input);
        REQUIRE(removeProblem._type == StorageCacheProblemType::NoProblems);
    }

    checkSelect(false);

    std::filesystem::remove(dbPath);
}
