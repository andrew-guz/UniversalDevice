#include "Settings.h"

#include <fstream>
#include <nlohmann/json.hpp>

#include "Defines.h"

Settings Settings::ReadSettings()
{
    std::ifstream settings_file_Stream(SETTINGS_FILE);
    auto settings_json = nlohmann::json::parse(settings_file_Stream);

    Settings settings;
    settings._port = settings_json["port"].get<int>();
    return settings;
}
