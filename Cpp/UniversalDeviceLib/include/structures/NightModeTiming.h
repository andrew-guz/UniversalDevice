#ifndef _NIGHT_MODE_TIMING_H_
#define _NIGHT_MODE_TIMING_H_

#include <chrono>

struct NightModeTiming final
{
    std::chrono::system_clock::time_point _start;
    std::chrono::system_clock::time_point _end;
};

#endif //_NIGHT_MODE_TIMING_H_
