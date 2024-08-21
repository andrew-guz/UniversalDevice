#include <catch2/catch_all.hpp>

#include "DbExtension.hpp"

std::vector<std::string> data{ "key1", "value1", "key2", "value2" };

TEST_CASE("Found") {
    REQUIRE(DbExtension::FindValueByName(data, "key1") == "value1");
    REQUIRE(DbExtension::FindValueByName(data, "key2") == "value2");
}

TEST_CASE("Not found") { REQUIRE(DbExtension::FindValueByName(data, "key3").empty()); }
