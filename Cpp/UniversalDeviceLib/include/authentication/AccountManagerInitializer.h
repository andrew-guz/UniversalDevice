#ifndef _ACCOUNT_MANAGE_INITIALIZER_H_
#define _ACCOUNT_MANAGE_INITIALIZER_H_

#include "IAccountManagerInitializer.h"

class AccountManagerInitializer final : public IAccountManagerInitializer {
public:
    virtual ~AccountManagerInitializer() = default;

    virtual void Initialize(std::vector<Account>& accounts) override;
};

#endif //_ACCOUNT_MANAGE_INITIALIZER_H_
