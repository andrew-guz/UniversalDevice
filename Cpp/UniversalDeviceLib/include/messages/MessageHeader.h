#ifndef _MESSAGE_HEADER_H_
#define _MESSAGE_HEADER_H_

#include "DeviceDescription.h"

struct MessageHeader final : public IJson<MessageHeader>
{
    DeviceDescription   _deviceDescription;
    std::string         _subject = Constants::SubjectUndefined;

    virtual nlohmann::json ToJson() const override
    {
        return {
            { "deviceDescription", _deviceDescription.ToJson() },
            { "subject", _subject }
        };
    }

    virtual void FromJson(const nlohmann::json& json) override
    {
        _deviceDescription.FromJson(json["deviceDescription"]);
        _subject = json.value("subject", Constants::SubjectUndefined);
    }
};

#endif //_MESSAGE_HEADER_H_
