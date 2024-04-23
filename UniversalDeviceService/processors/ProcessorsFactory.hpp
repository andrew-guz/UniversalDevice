#pragma once

#include "IProcessor.hpp"
#include "IQueryExecutor.hpp"

class ProcessorsFactory final {
public:
    static Processors CreateProcessors(const Message& message, IQueryExecutor* queryExecutor);
};
