#pragma once

#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

class ProcessSettingsReader final {
public:
    static nlohmann::json ReadProcessSettings();
};
