#include <catch2/catch_all.hpp>

#include <fmt/format.h>
#include <string>

#include "AccountManager.hpp"
#include "Base64Helper.hpp"

using namespace std::string_literals;

class AccountManagerInitializerStub final : public IAccountManagerInitializer {
public:
    virtual ~AccountManagerInitializerStub() = default;

    virtual void Initialize(std::vector<Account>& accounts) override {
        // TODO: formats is not working :(
        for (auto i = 1; i <= 5; ++i)
            accounts.emplace_back(fmt::format("login{}", i), fmt::format("password{}", i));
    }
};

TEST_CASE("IsValidUser") {
    AccountManager am{ std::make_shared<AccountManagerInitializerStub>() };
    REQUIRE(am.IsValidUser("login1", "password1"));
    REQUIRE_FALSE(am.IsValidUser("login2", "password23"));
    REQUIRE(am.IsValidUser(Account{ "login3", "password3" }));
    REQUIRE_FALSE(am.IsValidUser(Account{ "login4", "password45" }));
    REQUIRE(am.IsValidUser("Basic "s + Base64Helper::ToBase64("login5:password5")));
    REQUIRE_FALSE(am.IsValidUser("Basic "s + Base64Helper::ToBase64("login5:password")));
}

TEST_CASE("GetAuthString") {
    AccountManager am{ std::make_shared<AccountManagerInitializerStub>() };
    REQUIRE(am.GetAuthString("login1") == "login1:password1"s);
    REQUIRE(am.GetAuthString("login6") == std::string{});
}
