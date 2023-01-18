#ifndef _SERVICE_H_
#define _SERVICE_H_

#include <string>
#include <crow.h>

#include "Storage.h"

class Service
{
public:
    Service();

    ~Service();

public:
    std::string Version();

    int Inform(const crow::request& request);

    int Quit(crow::SimpleApp& app);

private:
    Storage _storage;
};

#endif //_SERVICE_H_
