#ifndef _MESSAGE_HEADER_H_
#define _MESSAGE_HEADER_H_

#include "ComponentDescription.h"

struct MessageHeader final : public IJson<MessageHeader>
{
    ComponentDescription    _from;
    ComponentDescription    _to;
    std::string             _subject = Constants::SubjectUndefined;

    virtual nlohmann::json ToJson() const override
    {
        return {
            { "from", _from.ToJson() },
            { "to", _to.ToJson() },
            { "subject", _subject }
        };
    }

    virtual void FromJson(const nlohmann::json& json) override
    {
        _from.FromJson(json["from"]);
        _to.FromJson(json["to"]);
        _subject = json.value("subject", Constants::SubjectUndefined);
    }
};

#endif //_MESSAGE_HEADER_H_
