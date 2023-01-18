#include "Settings.h"

#include "Defines.h"
#include "ProcessSettingsReader.h"

Settings Settings::ReadSettings()
{
    nlohmann::json settingsJson = ProcessSettingsReader::ReadProcessSettings();
    if (settingsJson.is_null())
        return Settings();

    Settings settings;
    settings._port = settingsJson.value("port", DEFAULT_PORT);
    return settings;
}
