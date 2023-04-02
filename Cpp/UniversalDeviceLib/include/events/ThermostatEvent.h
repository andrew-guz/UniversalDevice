#ifndef _THERMOSTAT_EVENT_H_
#define _THERMOSTAT_EVENT_H_

#include <limits>

#include "Event.h"

struct ThermostatEvent : public Event, public IJson<ThermostatEvent>
{
    float   _temperature = std::numeric_limits<float>::min();
    float   _delta = 0.5f;
    
    ThermostatEvent() :
        Event(Constants::EventTypeThermostat)
    {
        
    }

    virtual nlohmann::json ToJson() const override
    {
        auto event = Event::ToJson();
        event += { "temperature", _temperature };
        event += { "delta", _delta };
        return event;
    }

    virtual void FromJson(const nlohmann::json& json) override
    {
        Event::FromJson(json);
        _temperature = json.value("temperature", std::numeric_limits<float>::min());
        _delta = json.value("delta", 0.5f);
    }
};

#endif //_THERMOSTAT_EVENT_H_
