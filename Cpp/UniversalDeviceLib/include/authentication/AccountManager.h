#ifndef _ACCOUNT_MANAGER_H_
#define _ACCOUNT_MANAGER_H_

#include <vector>

#include "Singleton.h"
#include "Account.h"

class AccountManager final : public Singleton<AccountManager>
{
public:
    bool IsValidUser(const std::string& login, const std::string& password);

    bool IsValidUser(const Account& account);

    bool IsValidUser(const std::string& authorizationString);

    std::string GetAuthString(const std::string& login);

private:
    void Initialize();

    std::string ToBase64(const std::string& str);

    std::string FromBase64(const std::string& str);

private:
    std::vector<Account> _accounts;
};

#endif //_ACCOUNT_MANAGER_H_
