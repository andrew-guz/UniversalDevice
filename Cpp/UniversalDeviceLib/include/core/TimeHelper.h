#ifndef _TIME_HELPER_H_
#define _TIME_HELPER_H_

#include <string>
#include <chrono>
#include <tuple>

class TimeHelper final
{
public:
    static std::string TimeToString(const std::chrono::system_clock::time_point& time);

    static std::chrono::system_clock::time_point TimeFromString(const std::string& string);

    static std::tuple<int, int> GetHourMinute(const std::chrono::system_clock::time_point& time);
};

#endif //_TIME_HELPER_H_
