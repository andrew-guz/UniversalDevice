#include <filesystem>

#include <catch2/catch_all.hpp>

#include "IStorageCache.hpp"
#include "Scenario.hpp"
#include "SimpleTableStorageCache.hpp"
#include "Storage.hpp"

TEST_CASE("ScenariosTableStorageCacheTest") {
    std::filesystem::path dbPath = std::filesystem::temp_directory_path() / "test.db";
    if (std::filesystem::exists(dbPath))
        std::filesystem::remove(dbPath);

    Storage storage{ dbPath };

    IStorageCache* storageCache = GetScenariosCache(&storage);

    SimpleTableSelectAllOutput<Scenario> selectAllResult;
    const StorageCacheProblem selectAll = storageCache->SelectAll(selectAllResult);
    REQUIRE(selectAll._type == StorageCacheProblemType::NoProblems);
    REQUIRE(selectAllResult._data.size() == 0);

    std::filesystem::remove(dbPath);
}
