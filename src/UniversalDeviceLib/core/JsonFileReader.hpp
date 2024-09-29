#pragma once

#include <filesystem>
#include <string_view>

#include <nlohmann/json.hpp>

class JsonFileReader final {
public:
    static nlohmann::json ReadJson(const std::filesystem::path& fullFileName);
};
