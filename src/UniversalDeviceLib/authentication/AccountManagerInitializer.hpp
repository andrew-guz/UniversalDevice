#pragma once

#include <filesystem>
#include <mutex>

#include "IAccountManagerInitializer.hpp"

class AccountManagerInitializer final : public IAccountManagerInitializer {
public:
    AccountManagerInitializer(const std::filesystem::path& authPath);

    virtual ~AccountManagerInitializer() = default;

    virtual void Initialize(std::vector<Account>& accounts) override;

private:
    std::mutex _mutex;
    std::filesystem::path _authPath;
};
