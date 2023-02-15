#ifndef _EVENT_H_
#define _EVENT_H_

#include <string>

#include "Constants.h"
#include "IJson.h"
#include "ComponentDescription.h"

struct Event : public IJson<Event>
{
    std::string             _type;
    ComponentDescription    _provider;
    ComponentDescription    _receiver;
    nlohmann::json          _command;

    Event(const std::string& type = Constants::EventTypeUndefined) :
        _type(type)
    {
        
    }
    
    virtual nlohmann::json ToJson() const override
    {
        return {
            { "type", _type },
            { "provider", _provider.ToJson() },
            { "receiver", _receiver.ToJson() },
            { "command", _command }
        };  
    }

    virtual void FromJson(const nlohmann::json& json) override
    {
        _type = json.value("type", Constants::EventTypeUndefined);
        _provider.FromJson(json["provider"]);
        _receiver.FromJson(json["receiver"]);
        _command = json["command"];
    }
};

#endif //_EVENT_H_
