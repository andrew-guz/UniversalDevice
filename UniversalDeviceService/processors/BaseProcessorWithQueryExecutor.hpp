#pragma once

#include "IProcessor.hpp"
#include "IQueryExecutor.hpp"

class BaseProcessorWithQueryExecutor : public IProcessor {
public:
    BaseProcessorWithQueryExecutor(IQueryExecutor* queryExecutor);

    virtual ~BaseProcessorWithQueryExecutor() = default;

protected:
    IQueryExecutor* _queryExecutor;
};
