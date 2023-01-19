#include "TimeHelper.h"

#include <iomanip>

std::string TimeHelper::TimeToString(const std::chrono::system_clock::time_point& time)
{
    auto time_t = std::chrono::system_clock::to_time_t(time);
    std::stringstream timeStream;
    timeStream << std::put_time(std::localtime(&time_t), "%d-%m-%Y %T");
    timeStream.flush();
    return timeStream.str();
}
