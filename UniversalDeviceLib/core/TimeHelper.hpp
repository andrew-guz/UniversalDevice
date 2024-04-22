#pragma once

#include <chrono>
#include <string>
#include <string_view>
#include <tuple>

class TimeHelper final {
public:
    static int64_t TimeToInt(const std::chrono::system_clock::time_point& time);

    static std::chrono::system_clock::time_point TimeFromInt(int64_t value);

    // returns 25-11-2023 12:34:56 from time_point
    static std::string TimeToString(const std::chrono::system_clock::time_point& time);

    // returns 25-11-2023 12:34:56 from seconds since epoch
    static std::string TimeToString(int64_t value);

    // returns 25-11 12:34 from time_point
    static std::string TimeToShortString(const std::chrono::system_clock::time_point& time);

    static std::chrono::system_clock::time_point TimeFromString(std::string_view string);

    static std::tuple<int, int> GetHourMinute(const std::chrono::system_clock::time_point& time);
};
