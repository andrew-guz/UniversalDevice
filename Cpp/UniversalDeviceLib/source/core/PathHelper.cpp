#include "PathHelper.h"

#include <filesystem>

std::filesystem::path Path()
{
    return std::filesystem::canonical("/proc/self/exe");
}

std::string PathHelper::AppPath()
{
    return Path().c_str();
}

std::string PathHelper::AppDirPath()
{
    return Path().parent_path().c_str();
}
