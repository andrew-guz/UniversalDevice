#pragma once

#include <Wt/WApplication.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WStackedWidget.h>

#include "IStackHolder.hpp"
#include "Settings.hpp"

class Application final : public Wt::WApplication, public IStackHolder {
public:
    Application(const Settings& settings, const Wt::WEnvironment& env);

    virtual ~Application() = default;

    virtual void SetWidget(StackWidgetType type, const std::string& data) override;

private:
    Wt::WHBoxLayout* _mainLayout;
    Wt::WStackedWidget* _mainStack;
};
