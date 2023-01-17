#ifndef _REQUEST_ADDRESS_H_
#define _REQUEST_ADDRESS_H_

#include <string>

struct RequestAddress final
{
    std::string _ip;
    int         _port;
    std::string _api;

    std::string BuildUrl() const;
};

#endif //_REQUEST_ADDRESS_H_
