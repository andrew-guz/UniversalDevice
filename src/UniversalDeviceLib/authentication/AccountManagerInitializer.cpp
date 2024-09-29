#include "AccountManagerInitializer.hpp"

#include <mutex>

#include <nlohmann/json.hpp>

#include "Account.hpp"
#include "Enums.hpp"
#include "JsonFileReader.hpp"
#include "Marshaling.hpp"
#include "PathHelper.hpp"

AccountManagerInitializer::AccountManagerInitializer(const std::filesystem::path& authPath) :
    _authPath(authPath) {}

void AccountManagerInitializer::Initialize(std::vector<Account>& accounts) {
    std::lock_guard<std::mutex> lockGuard(_mutex);
    if (accounts.size())
        return;
    auto authenticationJson = JsonFileReader::ReadJson(_authPath);
    if (authenticationJson.is_null())
        return;
    auto serviceAccount = authenticationJson.value("serviceAccount", nlohmann::json()).get<Account>();
    serviceAccount._type = AccountType::Internal;
    accounts.push_back(serviceAccount);
    auto deviceAccount = authenticationJson.value("deviceAccount", nlohmann::json()).get<Account>();
    deviceAccount._type = AccountType::Internal;
    accounts.push_back(deviceAccount);
    auto usersJson = authenticationJson.value("users", nlohmann::json::array());
    for (auto& userJson : usersJson) {
        accounts.emplace_back(userJson.get<Account>());
    }
}
