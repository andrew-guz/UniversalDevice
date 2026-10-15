#include <memory>
#include <string>
#include <vector>

#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include <fmt/format.h>

#include "Account.hpp"
#include "AccountManager.hpp"
#include "Base64Helper.hpp"
#include "Enums.hpp"
#include "IAccountManagerInitializer.hpp"
#include <bits/basic_string.h>

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
    REQUIRE_FALSE(AccountManager::Instance()->IsValidAccount("login1", "password1"));
    REQUIRE(AccountManager::Instance()->IsValidAccount("login2", "password2"));
    REQUIRE_FALSE(AccountManager::Instance()->IsValidAccount("login2", "password23"));
    REQUIRE_FALSE(AccountManager::Instance()->IsValidAccount("Basic "s + Base64Helper::ToBase64("login1:password1")));
    REQUIRE(AccountManager::Instance()->IsValidAccount("Basic "s + Base64Helper::ToBase64("login2:password2")));
    REQUIRE_FALSE(AccountManager::Instance()->IsValidAccount("Basic "s + Base64Helper::ToBase64("login2:password23")));
}

TEST_CASE("GetAuthString") {
    AccountManager::Instance()->Init(std::make_shared<AccountManagerInitializerStub>());
    REQUIRE(AccountManager::Instance()->GetAuthString("login1") == "login1:password1"s);
    REQUIRE(AccountManager::Instance()->GetAuthString("login6") == std::string{});
}

TEST_CASE("CheckAccountTypes") {
    AccountManager::Instance()->Init(std::make_shared<AccountManagerInitializerStub>());
    REQUIRE(AccountManager::Instance()->GetAccountType("login1", "password1") == AccountType::Undefined);
    REQUIRE(AccountManager::Instance()->GetAccountType("login2", "password2") == AccountType::Viewer);
    REQUIRE(AccountManager::Instance()->GetAccountType("login3", "password3") == AccountType::User);
    REQUIRE(AccountManager::Instance()->GetAccountType("login4", "password4") == AccountType::Admin);
    REQUIRE(AccountManager::Instance()->GetAccountType("login5", "password5") == AccountType::Internal);
    REQUIRE(AccountManager::Instance()->GetAccountType("login6", "password6") == AccountType::Undefined);
}

TEST_CASE("CheckAccountTypesWrongPassword") {
    AccountManager::Instance()->Init(std::make_shared<AccountManagerInitializerStub>());
    REQUIRE(AccountManager::Instance()->GetAccountType("login4", "password5") == AccountType::Undefined);
    REQUIRE(AccountManager::Instance()->GetAccountType("login4", "") == AccountType::Undefined);
    REQUIRE(AccountManager::Instance()->GetAccountType("", "") == AccountType::Undefined);
}

TEST_CASE("ParseAuthorizationString") {
    AccountManager::Instance()->Init(std::make_shared<AccountManagerInitializerStub>());

    SECTION("valid account") {
        const auto [valid, login, password] =
            AccountManager::Instance()->ParseAuthorizationString("Basic "s + Base64Helper::ToBase64("login2:password2"));
        REQUIRE(valid);
        REQUIRE(login == "login2"s);
        REQUIRE(password == "password2"s);
    }

    SECTION("wrong password") {
        const auto [valid, login, password] =
            AccountManager::Instance()->ParseAuthorizationString("Basic "s + Base64Helper::ToBase64("login2:password23"));
        REQUIRE_FALSE(valid);
    }

    SECTION("account with undefined type") {
        const auto [valid, login, password] =
            AccountManager::Instance()->ParseAuthorizationString("Basic "s + Base64Helper::ToBase64("login1:password1"));
        REQUIRE_FALSE(valid);
    }

    SECTION("unknown account") {
        const auto [valid, login, password] =
            AccountManager::Instance()->ParseAuthorizationString("Basic "s + Base64Helper::ToBase64("login6:password6"));
        REQUIRE_FALSE(valid);
    }

    SECTION("no delimiter") {
        const auto [valid, login, password] =
            AccountManager::Instance()->ParseAuthorizationString("Basic "s + Base64Helper::ToBase64("login2password2"));
        REQUIRE_FALSE(valid);
    }

    SECTION("no Basic prefix") {
        const auto [valid, login, password] = AccountManager::Instance()->ParseAuthorizationString(Base64Helper::ToBase64("login2:password2"));
        REQUIRE_FALSE(valid);
    }

    SECTION("empty and too short strings") {
        for (const std::string& authorization : { ""s, "Basic"s, "Basic "s }) {
            REQUIRE_NOTHROW(AccountManager::Instance()->ParseAuthorizationString(authorization));
            const auto [valid, login, password] = AccountManager::Instance()->ParseAuthorizationString(authorization);
            REQUIRE_FALSE(valid);
        }
    }
}
