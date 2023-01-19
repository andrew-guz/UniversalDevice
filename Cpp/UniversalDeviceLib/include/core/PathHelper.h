#ifndef _PATH_HELPER_H_
#define _PATH_HELPER_H_

#include <string>

class PathHelper final
{
public:
    static std::string AppPath();

    static std::string AppDirPath();
};

#endif //_PATH_HELPER_H_
