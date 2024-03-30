#ifndef _BASE_PROCESSOR_USING_STORAGE_H_
#define _BASE_PROCESSOR_USING_STORAGE_H_

#include "IProcessor.h"
#include "IQueryExecutor.h"

class BaseProcessorWithQueryExecutor : public IProcessor {
public:
    BaseProcessorWithQueryExecutor(IQueryExecutor* queryExecutor);

    virtual ~BaseProcessorWithQueryExecutor() = default;

protected:
    IQueryExecutor* _queryExecutor;
};

#endif //_BASE_PROCESSOR_USING_STORAGE_H_
