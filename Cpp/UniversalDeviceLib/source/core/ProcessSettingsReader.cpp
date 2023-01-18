#include "ProcessSettingsReader.h"

#include <iostream>
#include <fstream>

#include "PathHelper.h"

nlohmann::json ProcessSettingsReader::ReadProcessSettings()
{
    try
    {
        auto settingsPath = PathHelper::AppPath() + std::string(".json");
        std::ifstream settingsFileStream(settingsPath);
        auto settingsJson = nlohmann::json::parse(settingsFileStream);
        return settingsJson;
    }
    catch (...)
    {
        std::cout << "Failed to read settings..." << std::endl;
    }

    return nlohmann::json();
}