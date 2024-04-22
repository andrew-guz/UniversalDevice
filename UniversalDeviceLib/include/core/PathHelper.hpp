#pragma once

#include <string>

class PathHelper final {
public:
    static std::string AppPath();

    static std::string AppDirPath();

    static std::string AppSettingsPath();

    static std::string AppLogPath();

    static std::string AppDbPath();

    static std::string FullFilePath(const std::string& shortFileName);
};
