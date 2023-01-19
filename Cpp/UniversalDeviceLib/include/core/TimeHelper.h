#ifndef _TIME_HELPER_H_
#define _TIME_HELPER_H_

#include <string>
#include <chrono>

class TimeHelper final
{
public:
    static std::string TimeToString(const std::chrono::_V2::system_clock::time_point& time);
};

#endif //_TIME_HELPER_H_