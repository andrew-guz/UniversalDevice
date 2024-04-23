#pragma once

#include <string_view>

class Base64Helper final {
public:
    static std::string ToBase64(std::string_view str);

    static std::string FromBase64(std::string_view str);
};
