#ifndef _I_ACCOUNT_MANAGE_INITIALIZER_H_
#define _I_ACCOUNT_MANAGE_INITIALIZER_H_

#include <vector>
#include "Account.h"

class IAccountManagerInitializer
{
public:
    virtual ~IAccountManagerInitializer() = default;

    virtual void Initialize(std::vector<Account>& accounts) = 0;
};

#endif //_I_ACCOUNT_MANAGE_INITIALIZER_H_
