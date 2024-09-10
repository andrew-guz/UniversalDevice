#include "AccountManager.hpp"

#include <algorithm>

#include "Base64Helper.hpp"
#include "Logger.hpp"

AccountManager::AccountManager(std::shared_ptr<IAccountManagerInitializer> initializer) :
    _initializer(initializer) {}

bool AccountManager::IsValidUser(const std::string& login, const std::string& password) {
    Account account;
    account._login = login;
    account._password = password;
    return IsValidUser(account);
}

bool AccountManager::IsValidUser(const Account& account) {
    Initialize();
    return std::any_of(_accounts.begin(), _accounts.end(), [&account](const auto& a) { return a == account; });
}

bool AccountManager::IsValidUser(const std::string_view authorizationString) {
    if (authorizationString.find("Basic") == std::string::npos)
        return false;
    auto base64 = authorizationString.substr(6);
    auto loginPassword = Base64Helper::FromBase64(base64);
    std::size_t delimeterIndex = loginPassword.find(":");
    if (delimeterIndex == std::string::npos)
        return false;
    auto login = loginPassword.substr(0, delimeterIndex);
    auto password = loginPassword.substr(delimeterIndex + 1);
    return IsValidUser(login, password);
}

std::string AccountManager::GetAuthString(const std::string_view login) {
    Initialize();
    if (auto iter = std::find_if(_accounts.begin(), _accounts.end(), [&login](const auto& account) { return account._login == login; });
        iter != _accounts.end())
        return iter->_login + std::string(":") + iter->_password;
    return {};
}

void AccountManager::Initialize() { _initializer->Initialize(_accounts); }
