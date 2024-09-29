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

    /// Path to executable log file (for example /home/user/Work/UniversalDevice/bin/UniversalDeviceService.log)
    static std::filesystem::path AppLogPath();

    /// Path to some file in AppDirPath
    static std::filesystem::path FullFilePath(const std::filesystem::path& path);
};
