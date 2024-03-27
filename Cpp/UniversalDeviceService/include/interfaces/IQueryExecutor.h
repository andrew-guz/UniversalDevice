#ifndef _I_QUERY_EXECUTOR_H_
#define _I_QUERY_EXECUTOR_H_

#include <string_view>
#include <vector>

#include "Logger.h"

class IQueryExecutor
{
public:
    IQueryExecutor() = default;

    virtual ~IQueryExecutor() = default;

    virtual bool Execute(std::string_view query) = 0;

    virtual bool Execute(std::string_view query, int(*callback)(void*, int, char**, char**)) = 0;

    virtual bool Select(std::string_view query, std::vector<std::vector<std::string>>& data) = 0;
};

#define LOG_SQL_ERROR(QUERY) LOG_ERROR << "Error in query: '" << QUERY << "'." << std::endl;

#endif //_I_QUERY_EXECUTOR_H_
