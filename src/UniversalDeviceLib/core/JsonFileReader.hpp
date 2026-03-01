#pragma once

#include <filesystem>

#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

class JsonFileReader final {
public:
    static nlohmann::json ReadJson(const std::filesystem::path& fullFileName);
};
