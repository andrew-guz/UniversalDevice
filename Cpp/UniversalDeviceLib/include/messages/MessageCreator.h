#ifndef _MESSAGE_CREATOR_H_
#define _MESSAGE_CREATOR_H_

#include "Message.h"

class MessageCreator
{
public:
    static Message Create(const std::string& type, const Uuid& id, const std::string subject, const nlohmann::json data);
};

#endif //_MESSAGE_CREATOR_H_
