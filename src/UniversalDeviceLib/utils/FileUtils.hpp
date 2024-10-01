#pragma once

#include <filesystem>
#include <string>

#include "LogInformation.hpp"

std::string ReadLogFile(const std::filesystem::path& path);

LogInformation ReadApplicationLogFile();
