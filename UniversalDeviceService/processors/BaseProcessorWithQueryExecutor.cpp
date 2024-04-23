#include "BaseProcessorWithQueryExecutor.hpp"

BaseProcessorWithQueryExecutor::BaseProcessorWithQueryExecutor(IQueryExecutor* queryExecutor) : _queryExecutor(queryExecutor) {}
