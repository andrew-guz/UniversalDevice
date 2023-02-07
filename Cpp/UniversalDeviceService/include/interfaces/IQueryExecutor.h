#ifndef _I_QUERY_EXECUTOR_H_
#define _I_QUERY_EXECUTOR_H_

#include <string>
#include <vector>

#include "Logger.h"

class IQueryExecutor
{
public:
    IQueryExecutor() = default;

    virtual ~IQueryExecutor() = default;

    virtual bool Execute(const std::string& query) = 0;

    virtual bool Execute(const std::string& query, int(*callback)(void*, int, char**, char**)) = 0;

    virtual bool Select(const std::string& query, std::vector<std::vector<std::string>>& data) = 0;
};

#define LOG_SQL_ERROR(QUERY) LOG_ERROR << "Error in query: '" << QUERY << "'." << std::endl;

#endif //_I_QUERY_EXECUTOR_H_
