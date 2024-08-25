#include <catch2/catch_all.hpp>

#include <chrono>
#include <iomanip>

#include <fmt/chrono.h>
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
    REQUIRE(fmt::format("{:.1f}", 1.2345) == "1.2");
    REQUIRE(fmt::format("{:.3}", 1.2345) == "1.23");
    REQUIRE(fmt::format("{:.3f}", 1.2345) == "1.234");
    REQUIRE(fmt::format("{:.6}", 1.2345) == "1.2345");
    REQUIRE(fmt::format("{:.6f}", 1.2345) == "1.234500");
    REQUIRE(fmt::format("{:.1}", 29.185) == "3e+01");
    REQUIRE(fmt::format("{:.1f}", 29.185) == "29.2");
    REQUIRE(fmt::format("{:.1}", 9.185) == "9");
    REQUIRE(fmt::format("{:.1f}", 9.185) == "9.2");
    REQUIRE_THROWS(fmt::format("{:.3}", 5));
    REQUIRE_THROWS(fmt::format("{:.3f}", 5));
}

TEST_CASE("Format date") {
    const auto now = std::chrono::system_clock::now();

    const auto fmt_time_t = std::chrono::system_clock::to_time_t(now);
    const auto fmt_tm = fmt::localtime(fmt_time_t);
    const auto fmt_string = fmt::format("{:%d-%m-%Y %T}", fmt_tm);

    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = std::localtime(&time_t);
    std::stringstream sstream;
    sstream << std::put_time(tm, "%d-%m-%Y %T");
    sstream.flush();
    const auto put_time_string = sstream.str();

    REQUIRE(fmt_string == put_time_string);
}
