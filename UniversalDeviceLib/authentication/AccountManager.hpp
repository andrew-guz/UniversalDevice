#pragma once

#include <memory>
#include <vector>

#include "Account.hpp"
#include "AccountManagerInitializer.hpp"
#include "Singleton.hpp"

class AccountManager final : public Singleton<AccountManager> {
public:
    AccountManager(std::shared_ptr<IAccountManagerInitializer> initializer = std::make_shared<AccountManagerInitializer>());

    bool IsValidUser(std::string login, std::string password);

    bool IsValidUser(const Account& account);

    bool IsValidUser(std::string_view authorizationString);

    std::string GetAuthString(std::string_view login);

private:
    void Initialize();

private:
    std::shared_ptr<IAccountManagerInitializer> _initializer;
    std::vector<Account> _accounts;
};
