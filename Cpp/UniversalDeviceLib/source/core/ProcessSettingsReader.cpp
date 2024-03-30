#include "ProcessSettingsReader.h"

#include "JsonFileReader.h"
#include "Logger.h"

nlohmann::json ProcessSettingsReader::ReadProcessSettings() {
    try {
        auto settingsPath = PathHelper::AppSettingsPath();
        return JsonFileReader::ReadJson(settingsPath);
    } catch (...) {
        LOG_ERROR << "Failed to read settings." << std::endl;
    }

    return nlohmann::json();
}