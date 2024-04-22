#pragma once

#include "Account.hpp"
#include <vector>

class IAccountManagerInitializer {
public:
    virtual ~IAccountManagerInitializer() = default;

    virtual void Initialize(std::vector<Account>& accounts) = 0;
};
