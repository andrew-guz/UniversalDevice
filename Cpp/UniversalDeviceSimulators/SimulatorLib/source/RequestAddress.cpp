#include "RequestAddress.h"

#include <sstream> 

std::string RequestAddress::BuildUrl() const
{
    std::stringstream url;
    url << "http://" << _ip << ":" << _port << _api;
    return url.str();
}
