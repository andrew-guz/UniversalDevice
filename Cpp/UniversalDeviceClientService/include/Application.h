#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <Wt/WApplication.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WStackedWidget.h>

class Application final: public Wt::WApplication
{
public:
    Application(const Wt::WEnvironment& env);

    virtual ~Application() = default;

private:
    Wt::WHBoxLayout*    _mainLayout;
    Wt::WStackedWidget* _mainStack;
};


#endif //_APPLICATION_H_
