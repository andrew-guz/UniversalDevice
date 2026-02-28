#include "TimerService.hpp"

#include <chrono>
#include <functional>
#include <thread>

#include <nlohmann/json_fwd.hpp>

#include "Constants.hpp"
#include "Enums.hpp"
#include "MessageHelper.hpp"
#include "TimeHelper.hpp"

namespace {

    void TimerThreadFunction(std::function<void(void)> timerFunction) {
        auto [startHour, startMinute] = TimeHelper::GetHourMinute(std::chrono::system_clock::now());
        auto startValue = startHour * 60 + startMinute;
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            auto [currentHour, currentMinute] = TimeHelper::GetHourMinute(std::chrono::system_clock::now());
            auto currentValue = currentHour * 60 + currentMinute;
            if (currentValue != startValue) {
                timerFunction();
                startValue = currentValue;
            }
        }
    }

} // namespace

TimerService::TimerService() {
    // also start thread for timer events
    auto timerFunction = std::bind(&TimerService::TimerFunction, this);
    auto timingThread = new std::thread(TimerThreadFunction, timerFunction);
    timingThread->detach();
}

void TimerService::TimerFunction() {
    CallProcessorsNoResult(std::chrono::system_clock::now(),
                           MessageHelper::CreateEventMessage(Constants::PredefinedIdTimer, Subject::TimerEvent, nlohmann::json::object_t{}));
    CallProcessorsNoResult(std::chrono::system_clock::now(),
                           MessageHelper::CreateEventMessage(Constants::PredefinedIdSunrise, Subject::SunriseEvent, nlohmann::json::object_t{}));
    CallProcessorsNoResult(std::chrono::system_clock::now(),
                           MessageHelper::CreateEventMessage(Constants::PredefinedIdSunset, Subject::SunsetEvent, nlohmann::json::object_t{}));
}
