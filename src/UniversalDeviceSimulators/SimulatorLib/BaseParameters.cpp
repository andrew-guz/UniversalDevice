#include "BaseParameters.hpp"

#include <nlohmann/json_fwd.hpp>

#include "Defines.hpp"
#include "ProcessSettingsReader.hpp"
#include "Uuid.hpp"

BaseParameters BaseParameters::ReadFromFile() {
    nlohmann::json json = ProcessSettingsReader::ReadProcessSettings();
    if (json.is_null())
        return BaseParameters();

    BaseParameters parameters;
    parameters._port = json.value("port", DEFAULT_SERVICE_PORT);
    parameters._id = Uuid(json.value("id", ""));
    parameters._authPath = json.value("authPath", "");
    return parameters;
}
