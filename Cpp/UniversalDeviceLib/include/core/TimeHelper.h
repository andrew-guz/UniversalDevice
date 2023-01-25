#ifndef _TIME_HELPER_H_
#define _TIME_HELPER_H_

#include <string>
#include <chrono>

class TimeHelper final
{
public:
    static std::string TimeToString(const std::chrono::system_clock::time_point& time);

    static std::chrono::system_clock::time_point TimeFromString(const std::string& string);
};

#endif //_TIME_HELPER_H_
