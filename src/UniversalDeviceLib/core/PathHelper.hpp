#pragma once

#include <filesystem>
#include <string>

class PathHelper final {
public:
    static std::filesystem::path AppPath();

    static std::filesystem::path AppDirPath();

    static std::filesystem::path AppSettingsPath();

    static std::filesystem::path AppLogPath();

    static void SetAppDbPath(const std::filesystem::path& path);

    static std::filesystem::path AppDbPath();

    static std::filesystem::path FullFilePath(const std::string& shortFileName);

private:
    static std::filesystem::path _customDbPath;
};
