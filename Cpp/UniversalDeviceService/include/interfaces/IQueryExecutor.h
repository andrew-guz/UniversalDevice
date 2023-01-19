#ifndef _I_QUERY_EXECUTOR_H_
#define _I_QUERY_EXECUTOR_H_

#include <string>

class IQueryExecutor
{
public:
    IQueryExecutor() = default;

    virtual ~IQueryExecutor() = default;

    virtual void Execute(const std::string& query) = 0;

    virtual void Execute(const std::string& query, int(*callback)(void*, int, char**, char**)) = 0;
};

#endif //_I_QUERY_EXECUTOR_H_
