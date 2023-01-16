#include "Settings.h"

#include <fstream>
#include <nlohmann/json.hpp>

#include "Defines.h"

Settings Settings::ReadSettings()
{
    nlohmann::json settings_json;
    try
    {
        std::ifstream settings_file_stream(SETTINGS_FILE);
        settings_json = nlohmann::json::parse(settings_file_stream);    
    }
    catch(...)
    {
        return Settings();
    }

    Settings settings;
    settings._port = settings_json.value("port", DEFAULT_PORT);
    return settings;
}
