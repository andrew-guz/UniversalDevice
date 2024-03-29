#ifndef _THERMOMETER_EVENT_H_
#define _THERMOMETER_EVENT_H_

#include <limits>

#include "Event.h"

struct ThermometerEvent final : public Event, public IJson<ThermometerEvent>
{
    float   _temperature = std::numeric_limits<float>::min();
    bool    _lower = true;
    
    ThermometerEvent() :
        Event(Constants::EventTypeThermometer)
    {
        
    }
    
    virtual ~ThermometerEvent() = default;

    virtual nlohmann::json ToJson() const override
    {
        auto event = Event::ToJson();
        event += { "temperature", _temperature };
        event += { "lower", _lower };
        return event;
    }

    virtual void FromJson(const nlohmann::json& json) override
    {
        Event::FromJson(json);
        _temperature = json.value("temperature", std::numeric_limits<float>::min());
        _lower = json.value("lower", true);
    }
};

#endif //_THERMOMETER_EVENT_H_
