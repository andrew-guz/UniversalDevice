#include "LoginWidget.hpp"

#include <Wt/Http/Cookie.h>
#include <Wt/WApplication.h>
#include <Wt/WDialog.h>
#include <Wt/WGlobal.h>
#include <Wt/WGridLayout.h>
#include <Wt/WGroupBox.h>
#include <Wt/WPushButton.h>

#include "AccountManager.hpp"
#include "Base64Helper.hpp"
#include "WidgetHelper.hpp"

using namespace Wt;

LoginWidget::LoginWidget(IStackHolder* stackHolder, const Settings& settings) :
    BaseStackWidget(stackHolder, settings) {
    auto mainLayout = setLayout(std::make_unique<WGridLayout>());
    auto box = mainLayout->addWidget(std::make_unique<WGroupBox>(), 0, 0, AlignmentFlag::Center | AlignmentFlag::Top);
    box->setTitle("Вход");
    auto boxLayout = box->setLayout(std::make_unique<WGridLayout>());
    boxLayout->addWidget(std::make_unique<WText>("Логин:"), 0, 0, AlignmentFlag::Left);
    _login = boxLayout->addWidget(std::make_unique<WLineEdit>(), 0, 1);
    _login->enterPressed().connect([this]() { Login(); });
    boxLayout->addWidget(std::make_unique<WText>("Пароль:"), 1, 0, AlignmentFlag::Left);
    _password = boxLayout->addWidget(std::make_unique<WLineEdit>(), 1, 1);
    _password->setEchoMode(EchoMode::Password);
    _password->enterPressed().connect([this]() { Login(); });
    auto okBtn = boxLayout->addWidget(std::make_unique<WPushButton>("Вход"), 2, 1, AlignmentFlag::Right);
    okBtn->clicked().connect([this]() { Login(); });
    okBtn->enterPressed().connect([this]() { Login(); });
}

void LoginWidget::Initialize(const std::string& data) {
    _login->setText({});
    _password->setText({});
    if (data.size()) {
        auto decoded = Base64Helper::FromBase64(data);
        auto delimeter = decoded.find(":");
        if (delimeter != std::string::npos) {
            _login->setText(decoded.substr(0, delimeter));
            _password->setText(decoded.substr(delimeter + 1));
            Login();
        }
    }
}

void LoginWidget::Login() {
    auto login = _login->text().toUTF8();
    auto password = _password->text().toUTF8();
    if (AccountManager::Instance()->IsValidUser(login, password)) {
        auto data = login + ":" + password;
        auto encoded = Base64Helper::ToBase64(data);
        Http::Cookie authorizationCookie("authorization");
        authorizationCookie.setMaxAge(std::chrono::seconds(60 * 60 * 24 * 10));
        authorizationCookie.setSecure(true);
        authorizationCookie.setValue(encoded);
        WApplication::instance()->setCookie(authorizationCookie);
        _stackHolder->SetWidget(StackWidgetType::Devices, {});
        return;
    }
    WidgetHelper::ShowSimpleMessage(this, "Ошибка", "Неизвестный пользователь!");
}
