#pragma once

#include <filesystem>

#include "Defines.hpp"

struct Settings {
    int _servicePort = DEFAULT_SERVICE_PORT;
    int _frontendPort = DEFAULT_FRONTEND_PORT;
    std::filesystem::path _certificatePath;
    std::filesystem::path _keyPath;
    std::filesystem::path _dhPath;
    std::filesystem::path _authPath;
    std::filesystem::path _logPath;

    static Settings ReadSettings();
};
