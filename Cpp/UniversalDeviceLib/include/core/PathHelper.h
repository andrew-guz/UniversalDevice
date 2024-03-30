#ifndef _PATH_HELPER_H_
#define _PATH_HELPER_H_

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

#endif //_PATH_HELPER_H_
