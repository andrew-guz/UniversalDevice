#pragma once

#include <string>

#include <Wt/WApplication.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WStackedWidget.h>

#include "ApplicationSettings.hpp"
#include "Enums.hpp"
#include "IStackHolder.hpp"

class Application final : public Wt::WApplication, public IStackHolder {
public:
    Application(const ApplicationSettings& settings, const Wt::WEnvironment& env);

    virtual ~Application() = default;

    virtual void SetWidget(StackWidgetType type, const std::string& data) override;

    virtual AccountType GetAccountType() const override;

    virtual void SetAccountType(AccountType accountType) override;

private:
    Wt::WHBoxLayout* _mainLayout;
    Wt::WStackedWidget* _mainStack;
    AccountType _accountType = AccountType::Undefined;
};
