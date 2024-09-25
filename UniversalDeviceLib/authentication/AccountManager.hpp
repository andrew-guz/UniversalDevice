#pragma once

#include <memory>
#include <vector>

#include "Account.hpp"
#include "AccountManagerInitializer.hpp"
#include "Enums.hpp"
#include "Singleton.hpp"

class AccountManager final : public Singleton<AccountManager> {
public:
    AccountManager(std::shared_ptr<IAccountManagerInitializer> initializer = std::make_shared<AccountManagerInitializer>());

    bool IsValidUser(const std::string& login, const std::string& password);

    bool IsValidUser(std::string_view authorizationString);

    std::string GetAuthString(std::string_view login);

    AccountType GetUserType(std::string_view login) const;

private:
    std::shared_ptr<IAccountManagerInitializer> _initializer;
    std::vector<Account> _accounts;
};
