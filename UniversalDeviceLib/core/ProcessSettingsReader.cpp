#include "ProcessSettingsReader.hpp"

#include "JsonFileReader.hpp"
#include "Logger.hpp"

nlohmann::json ProcessSettingsReader::ReadProcessSettings() {
    try {
        auto settingsPath = PathHelper::AppSettingsPath();
        return JsonFileReader::ReadJson(settingsPath);
    } catch (...) {
        LOG_ERROR_MSG("Failed to read settings.");
    }

    return nlohmann::json();
}
