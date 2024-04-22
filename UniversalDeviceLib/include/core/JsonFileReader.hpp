#pragma once

#include <nlohmann/json.hpp>
#include <string_view>

class JsonFileReader final {
public:
    static nlohmann::json ReadJson(std::string_view fullFileName);
};
