#pragma once

#include <string>
#include <tuple>

#include "Enums.hpp"

struct Account final {
    std::string _login;
    std::string _password;
    AccountType _type;

    explicit Account(const std::string& login = {}, const std::string& password = {}, AccountType type = AccountType::Undefined) :
        _login(login),
        _password(password),
        _type(type) {}

    bool operator==(const Account& other) const { return std::tie(_login, _password, _type) == std::tie(other._login, other._password, _type); }
};
