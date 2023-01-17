#include "Parameters.h"

#include <fstream>
#include <nlohmann/json.hpp>

Parameters Parameters::ReadFromFile(const std::string fileName)
{
    nlohmann::json json;
    try
    {
        std::ifstream file_stream(fileName);
        json = nlohmann::json::parse(file_stream);
    }
    catch(...)
    {
        return Parameters();
    }

    Parameters parameters;
    parameters._port = json.value("port", 8080);
    parameters._id = Uuid(json.value("id", ""));
    parameters._startTemperature = json.value("startTemperature", 20.0f);
    parameters._delta = json.value("delta", 0.1f);
    parameters._sleepTimeout = json.value("sleepTimeout", 1);
    return parameters;
}