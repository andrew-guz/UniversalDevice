#pragma once

#include <filesystem>

#include "Defines.hpp"
#include "Uuid.hpp"

class BaseParameters {
public:
    BaseParameters() = default;

    static BaseParameters ReadFromFile();

public:
    int _port = DEFAULT_SERVICE_PORT;
    Uuid _id;
    std::filesystem::path _authPath;
};
