#include "catch.hpp"

#include "DbExtension.h"

using namespace std::string_view_literals;

std::vector<std::string> data
{
    "key1",
    "value1",
    "key2",
    "value2"
};

TEST_CASE("Found")
{
    REQUIRE(DbExtension::FindValueByName(data, "key1"sv) == "value1"sv);
    REQUIRE(DbExtension::FindValueByName(data, "key2"sv) == "value2"sv);
}

TEST_CASE("Not found")
{
    REQUIRE(DbExtension::FindValueByName(data, "key3"sv).empty());
}
