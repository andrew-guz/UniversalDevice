#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <Wt/WApplication.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WStackedWidget.h>

#include "IStackHolder.h"
#include "Settings.h"

class Application final: public Wt::WApplication, public IStackHolder
{
public:
    Application(const Settings& settings, const Wt::WEnvironment& env);

    virtual ~Application() = default;

    virtual void SetWidget(StackWidgetType type, void* data) override;

private:
    Wt::WHBoxLayout*    _mainLayout;
    Wt::WStackedWidget* _mainStack;
};


#endif //_APPLICATION_H_
