#include "AccountManager.hpp"

#include <algorithm>
#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <tuple>

#include <fmt/format.h>

#include "Base64Helper.hpp"
#include "Enums.hpp"
#include "IAccountManagerInitializer.hpp"

void AccountManager::Init(std::shared_ptr<IAccountManagerInitializer> initializer) {
    _initializer = initializer;
    initializer->Initialize(_accounts);
}

bool AccountManager::IsValidAccount(const std::string& login, const std::string& password) const {
    return std::any_of(_accounts.begin(), _accounts.end(), [&login, &password](const auto& a) {
        return a._type != AccountType::Undefined && std::tie(a._login, a._password) == std::tie(login, password);
    });
}

bool AccountManager::IsValidAccount(const std::string_view authorizationString) const {
    const auto [valid, _1, _2] = AccountManager::Instance()->ParseAuthorizationString(authorizationString);

    return valid;
}

std::string AccountManager::GetAuthString(const std::string_view login) const {
    if (auto iter = std::find_if(_accounts.begin(), _accounts.end(), [&login](const auto& account) { return account._login == login; });
        iter != _accounts.end())
        return iter->_login + std::string(":") + iter->_password;

    return {};
}

std::string AccountManager::GetBasicAuthString(const std::string_view login) const {
    return fmt::format("Basic {}", Base64Helper::ToBase64(AccountManager::Instance()->GetAuthString(login)));
}

AccountType AccountManager::GetAccountType(const std::string_view login, const std::string_view password) const {
    if (auto iter = std::find_if(_accounts.begin(),
                                 _accounts.end(),
                                 [login, password](const auto& account) { return account._login == login && account._password == password; });
        iter != _accounts.end())
        return iter->_type;

    return AccountType::Undefined;
}

std::tuple<bool, std::string, std::string> AccountManager::ParseAuthorizationString(const std::string_view authorizationString) const {
    if (authorizationString.find("Basic") == std::string::npos || authorizationString.size() <= 6)
        return std::make_tuple(false, std::string{}, std::string{});

    auto base64 = authorizationString.substr(6);
    auto loginPassword = Base64Helper::FromBase64(base64);
    std::size_t delimeterIndex = loginPassword.find(":");
    if (delimeterIndex == std::string::npos)
        return std::make_tuple(false, std::string{}, std::string{});

    auto login = loginPassword.substr(0, delimeterIndex);
    auto password = loginPassword.substr(delimeterIndex + 1);

    return std::make_tuple(AccountManager::Instance()->IsValidAccount(login, password), login, password);
}
