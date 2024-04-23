#include "catch.hpp"

#include "UrlHelper.hpp"

TEST_CASE("URL replace test") {
    REQUIRE(UrlHelper::Url("http://path/<stationId>", "<stationId>", "123") == "http://path/123");
    REQUIRE(UrlHelper::Url("http://path/<stationId>", "<station>", "123") == "http://path/<stationId>");
}
