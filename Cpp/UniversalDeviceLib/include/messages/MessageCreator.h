#ifndef _MESSAGE_CREATOR_H_
#define _MESSAGE_CREATOR_H_

#include "Message.h"

class MessageCreator final
{
public:
    static Message Create(const ComponentDescription& from, const ComponentDescription& to, const std::string subject, const nlohmann::json& data);
};

#endif //_MESSAGE_CREATOR_H_
