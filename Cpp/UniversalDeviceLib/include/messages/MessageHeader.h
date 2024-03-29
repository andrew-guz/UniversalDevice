#ifndef _MESSAGE_HEADER_H_
#define _MESSAGE_HEADER_H_

#include "ComponentDescription.h"

struct MessageHeader final : public IJson<MessageHeader>
{
    ComponentDescription    _description;
    std::string             _subject = std::string { Constants::SubjectUndefined };

    virtual nlohmann::json ToJson() const override
    {
        return {
            { "description", _description.ToJson() },
            { "subject", _subject }
        };
    }

    virtual void FromJson(const nlohmann::json& json) override
    {
        _description.FromJson(json["description"]);
        _subject = json.value("subject", Constants::SubjectUndefined);
    }
};

#endif //_MESSAGE_HEADER_H_
