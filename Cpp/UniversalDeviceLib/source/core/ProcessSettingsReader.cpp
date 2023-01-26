#include "ProcessSettingsReader.h"

#include "Logger.h"

nlohmann::json ProcessSettingsReader::ReadProcessSettings()
{
    try
    {
        auto settingsPath = PathHelper::AppSettingsPath();
        std::ifstream settingsFileStream(settingsPath);
        auto settingsJson = nlohmann::json::parse(settingsFileStream);
        return settingsJson;
    }
    catch (...)
    {
        LOG_ERROR << "Failed to read settings..." << std::endl;
    }

    return nlohmann::json();
}