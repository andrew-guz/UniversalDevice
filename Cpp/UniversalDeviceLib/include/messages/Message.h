#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <nlohmann/json.hpp>

#include "MessageHeader.h"

struct Message final
{
    MessageHeader   _header;

    nlohmann::json  _data;
};

#endif //_MESSAGE_H_
