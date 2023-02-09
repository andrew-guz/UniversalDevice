#include "LoginWidget.h"

#include <Wt/WGridLayout.h>
#include <Wt/WGroupBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WDialog.h>

#include "AccountManager.h"

using namespace Wt;

LoginWidget::LoginWidget(IStackHolder* stackHolder, const Settings& settings) :
    BaseStackWidget(stackHolder, settings)
{
    auto mainLayout = setLayout(std::make_unique<WGridLayout>());
    auto box = mainLayout->addWidget(std::make_unique<WGroupBox>(), 0, 0, AlignmentFlag::Center);
    box->setTitle("Вход");
    auto boxLayout = box->setLayout(std::make_unique<WGridLayout>());
    boxLayout->addWidget(std::make_unique<WText>("Логин:"), 0, 0, AlignmentFlag::Left);
    _login = boxLayout->addWidget(std::make_unique<WLineEdit>(), 0, 1);
    _login->enterPressed().connect([&](){
        Login();
    });
    boxLayout->addWidget(std::make_unique<WText>("Пароль:"), 1, 0, AlignmentFlag::Left);
    _password = boxLayout->addWidget(std::make_unique<WLineEdit>(), 1, 1);
    _password->setEchoMode(EchoMode::Password);
    _password->enterPressed().connect([&](){
        Login();
    });
    auto okBtn = boxLayout->addWidget(std::make_unique<WPushButton>("Вход"), 2, 1, AlignmentFlag::Right);
    okBtn->clicked().connect([&](){
        Login();
    });
    okBtn->enterPressed().connect([&](){
        Login();
    });
}

void LoginWidget::Initialize(const std::string& data)
{
    _login->setText({});
    _password->setText({});
}

void LoginWidget::Login()
{
    auto login = _login->text().toUTF8();
    auto password = _password->text().toUTF8();
    if (AccountManager::Instance()->IsValidUser(login, password))
    {
        _stackHolder->SetWidget(StackWidgetType::Devices, {});
        return;
    }
    auto dialog = addChild(std::make_unique<WDialog>("Ошибка"));
    auto layout = dialog->contents()->setLayout(std::make_unique<WGridLayout>());
    dialog->setMinimumSize(200, 100);
    dialog->setClosable(true);
    dialog->setResizable(false);
    dialog->rejectWhenEscapePressed(true);
    dialog->enterPressed().connect([&](){
        dialog->accept();
    });
    layout->addWidget(std::make_unique<WText>("Неизвестный пользователь!"), 0, 0, AlignmentFlag::Center);
    dialog->exec();
}
