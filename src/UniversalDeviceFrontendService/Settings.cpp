#include "Settings.hpp"

#include "Defines.hpp"
#include "ProcessSettingsReader.hpp"

Settings Settings::ReadSettings() {
    nlohmann::json settingsJson = ProcessSettingsReader::ReadProcessSettings();
    if (settingsJson.is_null())
        return Settings();

    Settings settings;
    settings._servicePort = settingsJson.value("servicePort", DEFAULT_SERVICE_PORT);
    settings._frontendPort = settingsJson.value("frontendPort", DEFAULT_FRONTEND_PORT);
    return settings;
}
