#ifndef _RELAY_EVENT_H_
#define _RELAY_EVENT_H_

#include "Event.h"

struct RelayEvent final : public Event, public IJson<RelayEvent>
{
    int _state = 0;
    
    RelayEvent() :
        Event(Constants::EventTypeRelay)
    {
        
    }

    virtual ~RelayEvent() = default;

    virtual nlohmann::json ToJson() const override
    {
        auto event = Event::ToJson();
        event += { "state", _state };
        return event;
    }

    virtual void FromJson(const nlohmann::json& json) override
    {
        Event::FromJson(json);
        _state = json.value("state", 0);
    }
};

#endif //_RELAY_EVENT_H_
