#include "PathHelper.hpp"

namespace {

    std::filesystem::path Path() { return std::filesystem::canonical("/proc/self/exe"); }

} // namespace

std::filesystem::path PathHelper::_customLogPath;

std::filesystem::path PathHelper::AppPath() { return Path().c_str(); }

std::filesystem::path PathHelper::AppDirPath() { return Path().parent_path().c_str(); }

std::filesystem::path PathHelper::AppSettingsPath() { return AppPath().native() + ".json"; }

std::filesystem::path PathHelper::AppLogPath() {
    return PathHelper::_customLogPath.empty() ? std::filesystem::path{ AppPath().native() + ".log" } : PathHelper::_customLogPath;
}

void PathHelper::SetCustomLogPath(const std::filesystem::path& path) { PathHelper::_customLogPath = path; }

std::filesystem::path PathHelper::FullFilePath(const std::filesystem::path& path) { return AppDirPath() / path; }
