#ifndef _STORAGE_H_
#define _STORAGE_H_

#include <sqlite3.h>

class Storage
{
public:
    Storage();

    ~Storage();

private:
    sqlite3* _connection = nullptr;
};

#endif //_STORAGE_H_
