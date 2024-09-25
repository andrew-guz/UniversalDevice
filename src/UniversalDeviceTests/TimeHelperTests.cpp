#define CONFIG_CATCH_MAIN
#include <catch2/catch_all.hpp>

#include "TimeHelper.hpp"

TEST_CASE() {
    auto now = std::chrono::system_clock::now();

    auto intValue1 = TimeHelper::TimeToInt(now);
    auto timeValue = TimeHelper::TimeFromInt(intValue1);
    auto delta = now - timeValue;
    delta = std::chrono::duration_cast<std::chrono::seconds>(delta);
    REQUIRE(delta.count() == 0);

    auto intValue2 = TimeHelper::TimeToInt(timeValue);
    REQUIRE(intValue1 == intValue2);
}

TEST_CASE() {
    auto now = std::chrono::system_clock::now();

    auto stringValue1 = TimeHelper::TimeToString(now);
    auto timeValue = TimeHelper::TimeFromString(stringValue1);
    auto delta = now - timeValue;
    delta = std::chrono::duration_cast<std::chrono::seconds>(delta);
    REQUIRE(delta.count() == 0);

    auto stringValue2 = TimeHelper::TimeToString(timeValue);
    REQUIRE(stringValue1 == stringValue2);
}

TEST_CASE() {
    auto now = std::chrono::system_clock::now();

    auto intValue = TimeHelper::TimeToInt(now);
    auto stringValue1 = TimeHelper::TimeToString(now);
    auto stringValue2 = TimeHelper::TimeToString(intValue);
    REQUIRE(stringValue1 == stringValue2);
}

TEST_CASE() {
    auto time = TimeHelper::TimeFromString("01-01-2023 01:23:56");
    auto [hour, minute] = TimeHelper::GetHourMinute(time);
    REQUIRE(hour == 1);
    REQUIRE(minute == 23);
}
