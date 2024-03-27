#ifndef _ACCOUNT_H_
#define _ACCOUNT_H_

#include "IJson.h"

struct Account final : public IJson<Account>
{
    std::string _login;
    std::string _password;

    explicit Account(const std::string& login = {}, const std::string& password = {}) :
        _login(login),
        _password(password)
    {

    }

    virtual nlohmann::json ToJson() const override
    {
        return {
            { "login", _login },
            { "password", _password }
        };  
    }

    virtual void FromJson(const nlohmann::json& json) override
    {
        _login = json.value("login", std::string());
        _password = json.value("password", std::string());
    }

    bool operator==(const Account& other) const
    {
        return std::tie(_login, _password) == std::tie(other._login, other._password);
    }
};

#endif //_ACCOUNT_H_
