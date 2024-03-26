#include "JsonFileReader.h"

#include "Logger.h"

nlohmann::json JsonFileReader::ReadJson(const std::string_view fullFileName)
{
    try
    {
        std::ifstream settingsFileStream(fullFileName.data());
        auto settingsJson = nlohmann::json::parse(settingsFileStream);
        return settingsJson;
    }
    catch (...)
    {
        LOG_ERROR << "Failed to read file." << std::endl;
    }

    return nlohmann::json();
}
