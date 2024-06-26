#include "Settings.hpp"

#include "Defines.hpp"
#include "ProcessSettingsReader.hpp"

Settings Settings::ReadSettings() {
    nlohmann::json settingsJson = ProcessSettingsReader::ReadProcessSettings();
    if (settingsJson.is_null())
        return Settings();

    Settings settings;
    settings._port = settingsJson.value("port", DEFAULT_SERVICE_PORT);
    return settings;
}
