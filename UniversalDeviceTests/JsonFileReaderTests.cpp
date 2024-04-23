#include "catch.hpp"

#include "JsonFileReader.hpp"

TEST_CASE("Read valid file") {
    auto json = JsonFileReader::ReadJson("valid.json");
    REQUIRE_FALSE(json.is_null());
    REQUIRE(json.contains("key"));
    REQUIRE(json.value("key", "") == "value");
}

TEST_CASE("Read invalid file") {
    auto json = JsonFileReader::ReadJson("invalid.json");
    REQUIRE(json.is_null());
}

TEST_CASE("Read not existing file") {
    auto json = JsonFileReader::ReadJson("not_existing.json");
    REQUIRE(json.is_null());
}
