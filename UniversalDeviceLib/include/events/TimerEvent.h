#ifndef _TIMER_EVENT_H_
#define _TIMER_EVENT_H_

#include "Event.h"

struct TimerEvent final : public Event, public IJson<TimerEvent> {
    int _hour = 0;
    int _minute = 0;

    TimerEvent() : Event(Constants::EventTypeTimer) {}

    virtual ~TimerEvent() = default;

    virtual nlohmann::json ToJson() const override {
        auto event = Event::ToJson();
        event += {"hour", _hour};
        event += {"minute", _minute};
        return event;
    }

    virtual void FromJson(const nlohmann::json& json) override {
        Event::FromJson(json);
        _hour = json.value("hour", 0);
        _minute = json.value("minute", 0);
    }
};

#endif //_TIMER_EVENT_H_
