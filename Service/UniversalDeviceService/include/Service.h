#ifndef _SERVICE_H_
#define _SERVICE_H_

#include <string>
#include <crow.h>

class Service
{
public:
    static std::string Version();

    static int Quit(crow::SimpleApp& app);
};

#endif //_SERVICE_H_
