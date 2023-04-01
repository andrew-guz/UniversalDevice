#include "TimeHelper.h"

#include <iomanip>
#include <time.h>

std::string TimeHelper::TimeToString(const std::chrono::system_clock::time_point& time)
{
    auto time_t = std::chrono::system_clock::to_time_t(time);
    auto tm = std::localtime(&time_t);
    std::stringstream sstream;
    sstream << std::put_time(tm, "%d-%m-%Y %T");
    sstream.flush();
    return sstream.str();

}

std::string TimeHelper::TimeToShortString(const std::chrono::system_clock::time_point& time)
{
    auto time_t = std::chrono::system_clock::to_time_t(time);
    auto tm = std::localtime(&time_t);
    std::stringstream sstream;
    sstream << std::put_time(tm, "%H:%M %d-%m");
    sstream.flush();
    return sstream.str();

}

std::chrono::system_clock::time_point TimeHelper::TimeFromString(const std::string& string)
{
    std::tm tm = {};
    std::stringstream sstream(string);
    sstream >> std::get_time(&tm, "%d-%m-%Y %T");
    auto time_t = std::mktime(&tm);
    return std::chrono::system_clock::from_time_t(time_t);
}

std::tuple<int, int> TimeHelper::GetHourMinute(const std::chrono::system_clock::time_point& time)
{
    auto time_t = std::chrono::system_clock::to_time_t(time);
    auto tm = std::localtime(&time_t);
    return std::make_tuple(tm->tm_hour, tm->tm_min);
}
