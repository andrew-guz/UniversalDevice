#ifndef _MESSAGE_HEADER_H_
#define _MESSAGE_HEADER_H_

#include <string>

#include "Uuid.h"

struct MessageHeader final
{
    std::string _from_type;
    Uuid        _from_id;
    std::string _subject;
};

#endif //_MESSAGE_HEADER_H_
