#pragma once

#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

struct Account;

void to_json(nlohmann::json& json, const Account& account);

void from_json(const nlohmann::json& json, Account& account);
