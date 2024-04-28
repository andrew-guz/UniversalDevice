#include "Marshaling.hpp"

#include "Account.hpp"

void to_json(nlohmann::json& json, const Account& account) {
    json = {
        { "login", account._login },
        { "password", account._password },
    };
}

void from_json(const nlohmann::json& json, Account& account) {
    account._login = json.value("login", std::string());
    account._password = json.value("password", std::string());
}
