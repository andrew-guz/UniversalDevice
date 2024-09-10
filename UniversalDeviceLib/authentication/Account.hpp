#pragma once

#include <string>
#include <tuple>

struct Account final {
    std::string _login;
    std::string _password;

    explicit Account(const std::string& login = {}, const std::string& password = {}) :
        _login(login),
        _password(password) {}

    bool operator==(const Account& other) const { return std::tie(_login, _password) == std::tie(other._login, other._password); }
};
