#pragma once

#include <regex>
#include <string>

#include "Defines.hpp"
#include "Uuid.hpp"

class UrlHelper final {
public:
    static std::string Url(const std::string& url, const std::string& before, const std::string& after) {
        return std::regex_replace(url, std::regex(before), after);
    }
};
