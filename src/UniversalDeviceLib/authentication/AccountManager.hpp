#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "Account.hpp"
#include "Enums.hpp"
#include "IAccountManagerInitializer.hpp"
#include "Singleton.hpp"

class AccountManager final : public Singleton<AccountManager> {
public:
    AccountManager() = default;

    void Init(std::shared_ptr<IAccountManagerInitializer> initializer);

    bool IsValidAccount(const std::string& login, const std::string& password);

    bool IsValidAccount(std::string_view authorizationString);

    std::string GetAuthString(std::string_view login);

    AccountType GetAccountType(std::string_view login) const;

private:
    std::shared_ptr<IAccountManagerInitializer> _initializer;
    std::vector<Account> _accounts;
};
