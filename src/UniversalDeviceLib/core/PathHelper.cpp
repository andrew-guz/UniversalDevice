#include "PathHelper.hpp"

namespace {

    std::filesystem::path Path() { return std::filesystem::canonical("/proc/self/exe"); }

} // namespace

std::filesystem::path PathHelper::AppPath() { return Path().c_str(); }

std::filesystem::path PathHelper::AppDirPath() { return Path().parent_path().c_str(); }

std::filesystem::path PathHelper::AppSettingsPath() { return AppPath().native() + ".json"; }

std::filesystem::path PathHelper::AppLogPath() { return AppPath().native() + ".log"; }

std::filesystem::path PathHelper::FullFilePath(const std::filesystem::path& path) { return AppDirPath() / path; }
