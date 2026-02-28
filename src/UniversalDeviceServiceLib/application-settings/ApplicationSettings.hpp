#pragma once

#include <filesystem>

#include "Defines.hpp"
#include "Logger.hpp"

struct ApplicationSettings {
    int _port = DEFAULT_SERVICE_PORT;
    std::filesystem::path _dbPath;
    std::filesystem::path _certificatePath;
    std::filesystem::path _keyPath;
    std::filesystem::path _authPath;
    std::filesystem::path _logPath;
    LogLevel _logLevel = LogLevel::INFO;

    static ApplicationSettings ReadSettings();
};
