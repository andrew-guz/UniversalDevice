#ifndef _BASE_PROCESSOR_USING_STORAGE_H_
#define _BASE_PROCESSOR_USING_STORAGE_H_

#include "IProcessor.h"
#include "Storage.h"

class BaseProcessorUsingStorage : public IProcessor
{
public:
    BaseProcessorUsingStorage(Storage& storage);

    virtual ~BaseProcessorUsingStorage() = default;

protected:
    Storage& _storage;
};

#endif //_BASE_PROCESSOR_USING_STORAGE_H_
