#include "TimeHelper.hpp"

#include <fmt/chrono.h>
#include <fmt/format.h>
#include <iomanip>
#include <time.h>

int64_t TimeHelper::TimeToInt(const std::chrono::system_clock::time_point& time) {
    auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(time);
    auto secondsFromEpoch = seconds.time_since_epoch();
    return secondsFromEpoch.count();
}

std::chrono::system_clock::time_point TimeHelper::TimeFromInt(int64_t value) {
    auto seconds = std::chrono::seconds(value);
    return std::chrono::system_clock::time_point(seconds);
}

std::string TimeHelper::TimeToString(const std::chrono::system_clock::time_point& time) {
    const auto time_t = std::chrono::system_clock::to_time_t(time);
    const auto tm = fmt::localtime(time_t);
    return fmt::format("{:%d-%m-%Y %T}", tm);
}

std::string TimeHelper::TimeToString(int64_t value) { return TimeToString(TimeFromInt(value)); }

std::string TimeHelper::TimeToShortString(const std::chrono::system_clock::time_point& time) {
    const auto time_t = std::chrono::system_clock::to_time_t(time);
    const auto tm = fmt::localtime(time_t);
    return fmt::format("{:%H:%M %d-%m}", tm);
}

std::chrono::system_clock::time_point TimeHelper::TimeFromString(const std::string_view string) {
    std::tm tm = {};
    std::stringstream sstream(string.data());
    sstream >> std::get_time(&tm, "%d-%m-%Y %T");
    auto time_t = std::mktime(&tm);
    return std::chrono::system_clock::from_time_t(time_t);
}

std::tuple<int, int> TimeHelper::GetHourMinute(const std::chrono::system_clock::time_point& time) {
    auto time_t = std::chrono::system_clock::to_time_t(time);
    auto tm = std::localtime(&time_t);
    return std::make_tuple(tm->tm_hour, tm->tm_min);
}
