#include "Parameters.hpp"

#include "ProcessSettingsReader.h"

Parameters Parameters::ReadFromFile() {
    nlohmann::json json = ProcessSettingsReader::ReadProcessSettings();
    if (json.is_null())
        return Parameters();

    Parameters parameters;
    parameters._startTemperature = json.value("startTemperature", 20.0f);
    parameters._delta = json.value("delta", 0.1f);
    return parameters;
}
