#include "RequestAddress.hpp"

#include <string>

#include <fmt/format.h>

std::string RequestAddress::BuildUrl() const { return fmt::format("https://{}:{}{}", _ip, _port, _api); }
