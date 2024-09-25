#pragma once

#include <vector>

#include "Account.hpp"

class IAccountManagerInitializer {
public:
    virtual ~IAccountManagerInitializer() = default;

    virtual void Initialize(std::vector<Account>& accounts) = 0;
};
