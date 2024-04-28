#include "AccountManagerInitializer.hpp"

#include "Account.hpp"
#include "JsonExtension.hpp"
#include "JsonFileReader.hpp"
#include "Marshaling.hpp"
#include "PathHelper.hpp"

void AccountManagerInitializer::Initialize(std::vector<Account>& accounts) {
    if (accounts.size())
        return;
    auto fileName = PathHelper::FullFilePath("authentication.json");
    auto authenticationJson = JsonFileReader::ReadJson(fileName);
    if (authenticationJson.is_null())
        return;
    auto serviceAccount = authenticationJson.value("serviceAccount", nlohmann::json()).get<Account>();
    accounts.push_back(serviceAccount);
    auto deviceAccount = authenticationJson.value("deviceAccount", nlohmann::json()).get<Account>();
    accounts.push_back(deviceAccount);
    auto usersJson = authenticationJson.value("users", nlohmann::json::array());
    for (auto& userJson : usersJson)
        accounts.emplace_back(userJson.get<Account>());
}
