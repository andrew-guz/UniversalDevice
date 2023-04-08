#ifndef _STORAGE_CACHE_SHARED_DATA_H_
#define _STORAGE_CACHE_SHARED_DATA_H_

#include <string>

namespace StorageCacheSharedData
{

    enum ProblemType
    {
        NoProblems,
        Empty,
        NotExists,
        TooMany,
        SQLError
    };

    struct Problem
    {
        ProblemType _type;
        std::string _message;
    };

};

#endif //_STORAGE_CACHE_SHARED_DATA_H_
