#pragma once

#include <nlohmann/json.hpp>

class ProcessSettingsReader final {
public:
    static nlohmann::json ReadProcessSettings();
};
