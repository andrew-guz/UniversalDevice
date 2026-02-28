#pragma once

#include <mutex>

#include "IQueryExecutor.hpp"

class Controller {
protected:
    Controller(IQueryExecutor* queryExecutor);

    virtual ~Controller() = default;

protected:
    mutable std::mutex _mutex;
    IQueryExecutor* _queryExecutor;
};
