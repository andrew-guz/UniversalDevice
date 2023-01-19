#include "BaseProcessorWithQueryExecutor.h"

BaseProcessorWithQueryExecutor::BaseProcessorWithQueryExecutor(IQueryExecutor* queryExecutor) :
    _queryExecutor(queryExecutor)
{
}
