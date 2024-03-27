#ifndef _ACCOUNT_MANAGER_H_
#define _ACCOUNT_MANAGER_H_

#include <vector>
#include <memory>

#include "Singleton.h"
#include "Account.h"
#include "AccountManagerInitializer.h"

class AccountManager final : public Singleton<AccountManager>
{
public:
    AccountManager(std::shared_ptr<IAccountManagerInitializer> initializer = std::make_shared<AccountManagerInitializer>());

    bool IsValidUser(const std::string& login, const std::string& password);

    bool IsValidUser(const Account& account);

    bool IsValidUser(std::string_view authorizationString);

    std::string GetAuthString(std::string_view login);

private:
    void Initialize();

private:
    std::shared_ptr<IAccountManagerInitializer> _initializer;
    std::vector<Account>                        _accounts;
};

#endif //_ACCOUNT_MANAGER_H_
