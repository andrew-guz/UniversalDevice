#ifndef _MESSAGE_HEADER_H_
#define _MESSAGE_HEADER_H_

#include <string>

#include "IJson.h"
#include "Constants.h"
#include "Uuid.h"

struct MessageHeader final : public IJson<MessageHeader>
{
    std::string _type;
    Uuid        _id;
    std::string _subject;

    virtual nlohmann::json ToJson() const override
    {
        return {
            { "type", _type },
            { "id", _id.data() },
            { "subject", _subject }
        };
    }

    virtual void FromJson(const nlohmann::json& json) override
    {
        _type = json.value("type", Constants::DeviceTypeUndefined);
        _id = Uuid(json.value("id", ""));
        _subject = json.value("subject", Constants::SubjectUndefined);
    }
};

#endif //_MESSAGE_HEADER_H_
