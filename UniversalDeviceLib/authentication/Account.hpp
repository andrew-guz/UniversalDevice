#pragma once

#include <string>
#include <tuple>

struct Account final {
    std::string _login;
    std::string _password;

    explicit Account(std::string login = {}, std::string password = {}) : _login(std::move(login)), _password(std::move(password)) {}

    bool operator==(const Account& other) const { return std::tie(_login, _password) == std::tie(other._login, other._password); }
};
