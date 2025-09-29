#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPasswordEdit.h>

#include "ApplicationSettings.hpp"
#include "BaseStackWidget.hpp"

class LoginWidget final : public Wt::WContainerWidget, public BaseStackWidget {
public:
    LoginWidget(IStackHolder* stackHolder, const ApplicationSettings& settings);

    virtual ~LoginWidget() = default;

    virtual void Initialize(const std::string& data) override;

private:
    void Login();

private:
    Wt::WLineEdit* _login;
    Wt::WPasswordEdit* _password;
};
