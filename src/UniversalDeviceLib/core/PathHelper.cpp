#include "PathHelper.hpp"

namespace {

    std::filesystem::path Path() { return std::filesystem::canonical("/proc/self/exe"); }

} // namespace

std::filesystem::path PathHelper::_customDbPath;

std::filesystem::path PathHelper::AppPath() { return Path().c_str(); }

std::filesystem::path PathHelper::AppDirPath() { return Path().parent_path().c_str(); }

std::filesystem::path PathHelper::AppSettingsPath() { return AppPath().native() + ".json"; }

std::filesystem::path PathHelper::AppLogPath() { return AppPath().native() + ".log"; }

void PathHelper::SetAppDbPath(const std::filesystem::path& path) { _customDbPath = path; }

std::filesystem::path PathHelper::AppDbPath() {
    return _customDbPath.empty() ? std::filesystem::path{ AppPath().native() + ".db" } : PathHelper::_customDbPath;
}

std::filesystem::path PathHelper::FullFilePath(const std::string& shortFileName) { return AppDirPath() / shortFileName; }
