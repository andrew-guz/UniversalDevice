#ifndef _I_QUERY_EXECUTOR_H_
#define _I_QUERY_EXECUTOR_H_

#include <string>
#include <vector>

class IQueryExecutor
{
public:
    IQueryExecutor() = default;

    virtual ~IQueryExecutor() = default;

    virtual bool Execute(const std::string& query) = 0;

    virtual bool Execute(const std::string& query, int(*callback)(void*, int, char**, char**)) = 0;

    virtual bool Select(const std::string& query, std::vector<std::vector<std::string>>& data) = 0;
};

#endif //_I_QUERY_EXECUTOR_H_
