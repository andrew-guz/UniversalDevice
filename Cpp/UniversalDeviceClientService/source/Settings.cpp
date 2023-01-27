#include "Settings.h"

#include "Defines.h"
#include "ProcessSettingsReader.h"

Settings Settings::ReadSettings()
{
    nlohmann::json settingsJson = ProcessSettingsReader::ReadProcessSettings();
    if (settingsJson.is_null())
        return Settings();

    Settings settings;
    settings._servicePort = settingsJson.value("servicePort", DEFAULT_SERVICE_PORT);
    settings._clientPort = settingsJson.value("clientPort", DEFAULT_CLIENT_PORT);
    return settings;
}
