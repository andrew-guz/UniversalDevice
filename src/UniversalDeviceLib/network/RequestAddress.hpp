#pragma once

#include <string>

struct RequestAddress final {
    std::string _ip;
    int _port;
    std::string _api;

    std::string BuildUrl() const;
};
