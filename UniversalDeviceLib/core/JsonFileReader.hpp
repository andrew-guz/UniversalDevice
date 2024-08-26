#pragma once

#include <string_view>

#include <nlohmann/json.hpp>

class JsonFileReader final {
public:
    static nlohmann::json ReadJson(std::string_view fullFileName);
};
