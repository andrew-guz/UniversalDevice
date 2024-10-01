#include "Settings.hpp"

#include "Defines.hpp"
#include "ProcessSettingsReader.hpp"

Settings Settings::ReadSettings() {
    nlohmann::json settingsJson = ProcessSettingsReader::ReadProcessSettings();
    if (settingsJson.is_null())
        return Settings();

    Settings settings;
    settings._port = settingsJson.value("port", DEFAULT_SERVICE_PORT);
    settings._dbPath = settingsJson.value("dbPath", "");
    settings._certificatePath = settingsJson.value("certificatePath", "");
    settings._keyPath = settingsJson.value("keyPath", "");
    settings._authPath = settingsJson.value("authPath", "");
    settings._logPath = settingsJson.value("logPath", "");
    return settings;
}
