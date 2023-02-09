#include "JsonFileReader.h"

#include "Logger.h"

nlohmann::json JsonFileReader::ReadJson(const std::string& fullFileName)
{
    try
    {
        std::ifstream settingsFileStream(fullFileName);
        auto settingsJson = nlohmann::json::parse(settingsFileStream);
        return settingsJson;
    }
    catch (...)
    {
        LOG_ERROR << "Failed to read file." << std::endl;
    }

    return nlohmann::json();
}