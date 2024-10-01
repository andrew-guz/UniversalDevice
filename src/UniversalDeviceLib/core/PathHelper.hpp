#pragma once

#include <filesystem>

class PathHelper final {
public:
    /// Path to executable (for example /home/user/Work/UniversalDevice/bin/UniversalDeviceService)
    static std::filesystem::path AppPath();

    /// Path to executable folder (for example /home/user/Work/UniversalDevice/bin/UniversalDeviceService)
    static std::filesystem::path AppDirPath();

    /// Path to executable settings json file (for example /home/user/Work/UniversalDevice/bin/UniversalDeviceService.json)
    static std::filesystem::path AppSettingsPath();

    /// Path to executable log file (for example /home/user/Work/UniversalDevice/bin/UniversalDeviceService.log) or CustomLogPath if set
    static std::filesystem::path AppLogPath();

    static void SetCustomLogPath(const std::filesystem::path& path);

    /// Path to some file in AppDirPath
    static std::filesystem::path FullFilePath(const std::filesystem::path& path);

private:
    static std::filesystem::path _customLogPath;
};
