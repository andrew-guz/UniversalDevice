#ifndef _MESSAGE_SERIALIZER_H_
#define _MESSAGE_SERIALIZER_H_

#include "ISerializer.h"
#include "Message.h"

class MessageSerializer : ISerializer<Message>
{
public:
    virtual nlohmann::json ToJson(const Message& t) const override;

    virtual nlohmann::json ToJson(const std::shared_ptr<Message>& t) const override;

    virtual Message ToObject(const nlohmann::json& j) const override;

    virtual std::shared_ptr<Message> ToSharedObject(const nlohmann::json& j) const override;
};

#endif //_MESSAGE_SERIALIZER_H_