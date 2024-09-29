#pragma once

#include <filesystem>

#include "Defines.hpp"

struct Settings {
    int _port = DEFAULT_SERVICE_PORT;
    std::filesystem::path _dbPath;
    std::filesystem::path _certificatePath;
    std::filesystem::path _keyPath;
    std::filesystem::path _authPath;

    static Settings ReadSettings();
};
