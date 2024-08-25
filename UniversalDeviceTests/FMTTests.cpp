#include <catch2/catch_all.hpp>

#include <fmt/format.h>

TEST_CASE("Format time") {
    REQUIRE(fmt::format("{:02}:{:02}", 0, 0) == "00:00");
    REQUIRE(fmt::format("{:02}:{:02}", 0, 1) == "00:01");
    REQUIRE(fmt::format("{:02}:{:02}", 0, 59) == "00:59");
    REQUIRE(fmt::format("{:02}:{:02}", 1, 0) == "01:00");
    REQUIRE(fmt::format("{:02}:{:02}", 1, 59) == "01:59");
    REQUIRE(fmt::format("{:02}:{:02}", 23, 59) == "23:59");
}

TEST_CASE("Format float with precision") {
    REQUIRE(fmt::format("{:.1}", 1.2345) == "1");
    REQUIRE(fmt::format("{:.3}", 1.2345) == "1.23");
    REQUIRE(fmt::format("{:.6}", 1.2345) == "1.2345");
    REQUIRE_THROWS(fmt::format("{:.3}", 5));
}
