#include "TimerService.hpp"

#include "CurrentTime.hpp"
#include "MessageHelper.hpp"

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

TimerService::TimerService(IQueryExecutor* queryExecutor) :
    BaseService(queryExecutor) {}

void TimerService::Initialize(CrowApp& /* app */) {
    // also start thread for timer events
    auto timerFunction = std::bind(&TimerService::TimerFunction, this);
    auto timingThread = new std::thread(TimerThreadFunction, timerFunction);
    timingThread->detach();
}

void TimerService::TimerFunction() {
    CurrentTime currentTime;
    currentTime._timestamp = std::chrono::system_clock::now();
    auto message = MessageHelper::Create(DeviceType::Timer, Constants::PredefinedIdTimer, Subject::TimerEvent, currentTime);
    CallProcessorsNoResult(std::chrono::system_clock::now(), message);
}
