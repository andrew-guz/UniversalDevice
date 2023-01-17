#ifndef _MESSAGE_HEADER_H_
#define _MESSAGE_HEADER_H_

#include <string>

#include "Uuid.h"

struct MessageHeader final
{
    std::string _type;
    Uuid        _id;
    std::string _subject;
};

#endif //_MESSAGE_HEADER_H_
