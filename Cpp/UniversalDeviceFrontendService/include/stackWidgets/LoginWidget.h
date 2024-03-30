#ifndef _LOGIN_WIDGET_H_
#define _LOGIN_WIDGET_H_

#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>

#include "BaseStackWidget.h"
#include "Settings.h"

class LoginWidget final : public Wt::WContainerWidget, public BaseStackWidget {
public:
    LoginWidget(IStackHolder* stackHolder, const Settings& settings);

    virtual ~LoginWidget() = default;

    virtual void Initialize(const std::string& data) override;

private:
    void Login();

private:
    Wt::WLineEdit* _login;
    Wt::WLineEdit* _password;
};

#endif //_LOGIN_WIDGET_H_
