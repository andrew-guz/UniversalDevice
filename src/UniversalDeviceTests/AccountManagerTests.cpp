#include <string>

#include <catch2/catch_all.hpp>
#include <fmt/format.h>

#include "AccountManager.hpp"
#include "Base64Helper.hpp"
#include "Enums.hpp"

using namespace std::string_literals;

class AccountManagerInitializerStub final : public IAccountManagerInitializer {
public:
    virtual ~AccountManagerInitializerStub() = default;

    virtual void Initialize(std::vector<Account>& accounts) override {
        for (auto i = 1; i <= 5; ++i) {
            accounts.emplace_back(fmt::format("login{}", i), fmt::format("password{}", i), static_cast<AccountType>(i - 1));
        }
    }
};

TEST_CASE("IsValidUser") {
    AccountManager::Instance()->Init(std::make_shared<AccountManagerInitializerStub>());
    REQUIRE_FALSE(AccountManager::Instance()->IsValidUser("login1", "password1"));
    REQUIRE(AccountManager::Instance()->IsValidUser("login2", "password2"));
    REQUIRE_FALSE(AccountManager::Instance()->IsValidUser("login2", "password23"));
    REQUIRE_FALSE(AccountManager::Instance()->IsValidUser("Basic "s + Base64Helper::ToBase64("login1:password1")));
    REQUIRE(AccountManager::Instance()->IsValidUser("Basic "s + Base64Helper::ToBase64("login2:password2")));
    REQUIRE_FALSE(AccountManager::Instance()->IsValidUser("Basic "s + Base64Helper::ToBase64("login2:password23")));
}

TEST_CASE("GetAuthString") {
    AccountManager::Instance()->Init(std::make_shared<AccountManagerInitializerStub>());
    REQUIRE(AccountManager::Instance()->GetAuthString("login1") == "login1:password1"s);
    REQUIRE(AccountManager::Instance()->GetAuthString("login6") == std::string{});
}

TEST_CASE("CheckAccountTypes") {
    AccountManager::Instance()->Init(std::make_shared<AccountManagerInitializerStub>());
    REQUIRE(AccountManager::Instance()->GetUserType("login1") == AccountType::Undefined);
    REQUIRE(AccountManager::Instance()->GetUserType("login2") == AccountType::Viewer);
    REQUIRE(AccountManager::Instance()->GetUserType("login3") == AccountType::User);
    REQUIRE(AccountManager::Instance()->GetUserType("login4") == AccountType::Admin);
    REQUIRE(AccountManager::Instance()->GetUserType("login5") == AccountType::Internal);
    REQUIRE(AccountManager::Instance()->GetUserType("login6") == AccountType::Undefined);
}
