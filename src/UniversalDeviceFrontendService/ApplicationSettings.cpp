#include "ApplicationSettings.hpp"

#include "Defines.hpp"
#include "ProcessSettingsReader.hpp"

ApplicationSettings ApplicationSettings::ReadSettings() {
    nlohmann::json settingsJson = ProcessSettingsReader::ReadProcessSettings();
    if (settingsJson.is_null())
        return ApplicationSettings();

    ApplicationSettings settings;
    settings._servicePort = settingsJson.value("servicePort", DEFAULT_SERVICE_PORT);
    settings._frontendPort = settingsJson.value("frontendPort", DEFAULT_FRONTEND_PORT);
    settings._certificatePath = settingsJson.value("certificatePath", "");
    settings._keyPath = settingsJson.value("keyPath", "");
    settings._dhPath = settingsJson.value("dhPath", "");
    settings._authPath = settingsJson.value("authPath", "");
    settings._logPath = settingsJson.value("logPath", "");
    return settings;
}
