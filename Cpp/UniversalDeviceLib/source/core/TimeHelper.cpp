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

std::chrono::system_clock::time_point TimeHelper::TimeFromString(const std::string& string)
{
    std::tm tm;
    strptime(string.c_str(), "%d-%m-%Y %T", &tm);
    auto time_t = std::mktime(&tm);
    return std::chrono::system_clock::from_time_t(time_t);
}
