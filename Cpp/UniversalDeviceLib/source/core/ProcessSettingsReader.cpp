#include "ProcessSettingsReader.h"

#include <iostream>
#include <fstream>
#include <filesystem>

nlohmann::json ProcessSettingsReader::ReadProcessSettings()
{
    try
    {
        auto settingsPath = std::filesystem::canonical("/proc/self/exe").c_str() + std::string(".json");
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