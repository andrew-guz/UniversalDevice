#ifndef _MESSAGE_HEADER_SERIALIZER_H_
#define _MESSAGE_HEADER_SERIALIZER_H_

#include "ISerializer.h"
#include "MessageHeader.h"

class MessageHeaderSerializer final : public ISerializer<MessageHeader>
{
public:
    MessageHeaderSerializer() = default;

    virtual ~MessageHeaderSerializer() = default;

    virtual nlohmann::json ToJson(const MessageHeader& t) const override;

    virtual nlohmann::json ToJson(const std::shared_ptr<MessageHeader>& t) const override;

    virtual MessageHeader ToObject(const nlohmann::json& j) const override;

    virtual std::shared_ptr<MessageHeader> ToSharedObject(const nlohmann::json& j) const override;
};

#endif //_MESSAGE_HEADER_SERIALIZER_H_
