#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <Wt/WApplication.h>

class Application final: public Wt::WApplication
{
public:
    Application(const Wt::WEnvironment& env);

    virtual ~Application() = default;
};


#endif //_APPLICATION_H_
