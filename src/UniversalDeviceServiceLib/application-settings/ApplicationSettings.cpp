#include "ApplicationSettings.hpp"

#include "Defines.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "ProcessSettingsReader.hpp"

ApplicationSettings ApplicationSettings::ReadSettings() {
    nlohmann::json settingsJson = ProcessSettingsReader::ReadProcessSettings();
    if (settingsJson.is_null())
        return ApplicationSettings();

    ApplicationSettings settings;
    settings._port = settingsJson.value("port", DEFAULT_SERVICE_PORT);
    settings._dbPath = settingsJson.value("dbPath", "");
    settings._certificatePath = settingsJson.value("certificatePath", "");
    settings._keyPath = settingsJson.value("keyPath", "");
    settings._authPath = settingsJson.value("authPath", "");
    settings._logPath = settingsJson.value("logPath", "");
    settings._logLevel = settingsJson.contains("logLevel") ? settingsJson.at("logLevel").get<LogLevel>() : LogLevel::INFO;
    return settings;
}
