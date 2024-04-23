#include "AccountManagerInitializer.hpp"

#include "JsonExtension.hpp"
#include "JsonFileReader.hpp"
#include "PathHelper.hpp"

void AccountManagerInitializer::Initialize(std::vector<Account>& accounts) {
    if (accounts.size())
        return;
    auto fileName = PathHelper::FullFilePath("authentication.json");
    auto authenticationJson = JsonFileReader::ReadJson(fileName);
    if (authenticationJson.is_null())
        return;
    auto serviceAccount = JsonExtension::CreateFromJson<Account>(authenticationJson.value("serviceAccount", nlohmann::json()));
    accounts.push_back(serviceAccount);
    auto deviceAccount = JsonExtension::CreateFromJson<Account>(authenticationJson.value("deviceAccount", nlohmann::json()));
    accounts.push_back(deviceAccount);
    auto usersJson = authenticationJson.value("users", nlohmann::json::array());
    for (auto& userJson : usersJson) {
        auto userAccount = JsonExtension::CreateFromJson<Account>(userJson);
        accounts.push_back(userAccount);
    }
}
