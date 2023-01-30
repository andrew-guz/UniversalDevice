#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include "IJson.h"
#include "MessageHeader.h"

struct Message final : public IJson<Message>
{
    MessageHeader   _header;
    nlohmann::json  _data;

    bool IsValid() const
    {
        return  _header._from._type.size() &&
                _header._to._type.size() &&
                _header._subject.size() &&
                _header._subject != Constants::SubjectUndefined;
    }

    virtual nlohmann::json ToJson() const override
    {
        return {
            { "header", _header.ToJson() },
            { "data", _data }
        };
    }

    virtual void FromJson(const nlohmann::json& json) override
    {
        _header.FromJson(json["header"]);
        _data = json["data"];
    }
};

#endif //_MESSAGE_H_
