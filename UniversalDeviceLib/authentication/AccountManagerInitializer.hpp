#pragma once

#include "IAccountManagerInitializer.hpp"

class AccountManagerInitializer final : public IAccountManagerInitializer {
public:
    virtual ~AccountManagerInitializer() = default;

    virtual void Initialize(std::vector<Account>& accounts) override;
};
