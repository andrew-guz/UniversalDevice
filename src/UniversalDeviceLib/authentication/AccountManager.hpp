#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

#include "Account.hpp"
#include "Enums.hpp"
#include "IAccountManagerInitializer.hpp"
#include "Singleton.hpp"

class AccountManager final : public Singleton<AccountManager> {
public:
    AccountManager() = default;

    void Init(std::shared_ptr<IAccountManagerInitializer> initializer);

    bool IsValidAccount(const std::string& login, const std::string& password) const;

    bool IsValidAccount(std::string_view authorizationString) const;

    std::string GetAuthString(std::string_view login) const;

    std::string GetBasicAuthString(std::string_view login) const;

    AccountType GetAccountType(std::string_view login, std::string_view password) const;

    std::tuple<bool, std::string, std::string> ParseAuthorizationString(std::string_view authorizationString) const;

private:
    std::shared_ptr<IAccountManagerInitializer> _initializer;
    std::vector<Account> _accounts;
};
