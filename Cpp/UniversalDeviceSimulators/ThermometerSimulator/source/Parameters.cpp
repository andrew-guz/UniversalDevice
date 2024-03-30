#include "Parameters.h"

#include "ProcessSettingsReader.h"

Parameters Parameters::ReadFromFile(const std::string fileName) {
    nlohmann::json json = ProcessSettingsReader::ReadProcessSettings();
    if (json.is_null())
        return Parameters();

    Parameters parameters;
    parameters._port = json.value("port", DEFAULT_SERVICE_PORT);
    parameters._id = Uuid(json.value("id", ""));
    parameters._startTemperature = json.value("startTemperature", 20.0f);
    parameters._delta = json.value("delta", 0.1f);
    return parameters;
}