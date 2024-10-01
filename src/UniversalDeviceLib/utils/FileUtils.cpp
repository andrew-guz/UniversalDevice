#include "FileUtils.hpp"

#include <deque>
#include <fstream>
#include <numeric>

#include "Defines.hpp"
#include "PathHelper.hpp"

std::string ReadLogFile(const std::filesystem::path& path) {
    std::deque<std::string> lines;
    std::ifstream fileStream(path);
    std::string str;
    while (std::getline(fileStream, str))
        lines.push_front(str);
    if (lines.size() > LOG_RECORDS_COUT)
        lines.resize(LOG_RECORDS_COUT);
    return std::accumulate(lines.begin(), lines.end(), std::string{}, [](const auto& a, const auto& b) { return a + b + "\n"; });
}

LogInformation ReadApplicationLogFile() {
    const std::filesystem::path logPath = PathHelper::AppLogPath();
    return LogInformation{
        ._fileName = logPath.filename().string(),
        ._fileContent = ReadLogFile(logPath),
    };
}
