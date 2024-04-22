#include "RequestAddress.hpp"

#include <sstream>

std::string RequestAddress::BuildUrl() const {
    std::stringstream url;
    url << "https://" << _ip << ":" << _port << _api;
    url.flush();
    return url.str();
}
