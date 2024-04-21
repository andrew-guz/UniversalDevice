#include "BaseParameters.hpp"

#include "ProcessSettingsReader.h"

BaseParameters BaseParameters::ReadFromFile() {
    nlohmann::json json = ProcessSettingsReader::ReadProcessSettings();
    if (json.is_null())
        return BaseParameters();

    BaseParameters parameters;
    parameters._id = Uuid(json.value("id", ""));
    return parameters;
}
