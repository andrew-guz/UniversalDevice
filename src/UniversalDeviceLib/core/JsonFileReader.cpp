#include "JsonFileReader.hpp"

#include "Logger.hpp"

nlohmann::json JsonFileReader::ReadJson(const std::filesystem::path& fullFileName) {
    try {
        std::ifstream settingsFileStream(fullFileName.native());
        auto settingsJson = nlohmann::json::parse(settingsFileStream);
        return settingsJson;
    } catch (...) {
        LOG_ERROR_MSG("Failed to read file");
    }

    return nlohmann::json();
}
