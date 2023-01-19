#ifndef _STORAGE_PROCESSOR_H_
#define _STORAGE_PROCESSOR_H_

#include "IProcessor.h"
#include "Storage.h"

class StorageProcessor : public IProcessor
{
public:
    StorageProcessor(Storage& storage);

    virtual ~StorageProcessor() = default;

protected:
    Storage& _storage;
};

#endif //_STORAGE_PROCESSOR_H_
