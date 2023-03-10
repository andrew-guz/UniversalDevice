#include "AccountManager.h"

#include <sstream>
#include <nlohmann/json.hpp>
#include <b64/encode.h>
#include <b64/decode.h>

#include "Logger.h"
#include "PathHelper.h"
#include "JsonFileReader.h"

bool AccountManager::IsValidUser(const std::string& login, const std::string& password)
{
    Account account;
    account._login = login;
    account._password = password;
    return IsValidUser(account);
}

bool AccountManager::IsValidUser(const Account& account)
{
    Initialize();
    return std::find(_accounts.begin(), _accounts.end(), account) != _accounts.end();
}

bool AccountManager::IsValidUser(const std::string& authorizationString)
{
    if (authorizationString.find("Basic") == std::string::npos)
        return false;
    auto base64 = authorizationString.substr(6);
    auto loginPassword = FromBase64(base64);
    std::size_t delimeterIndex = loginPassword.find(":");
    if (delimeterIndex == std::string::npos)
        return false;
    auto login = loginPassword.substr(0, delimeterIndex);
    auto password = loginPassword.substr(delimeterIndex + 1);
    return IsValidUser(login, password);
}

std::string AccountManager::GetAuthString(const std::string& login)
{
    Initialize();
    auto iter = std::find_if(_accounts.begin(), _accounts.end(), [login](const auto& account){ return account._login == login; });
    if(iter == _accounts.end())
        return {};
    return iter->_login + std::string(":") + iter->_password;
}

void AccountManager::Initialize()
{
    if (_accounts.size())
        return;
    auto fileName = PathHelper::FullFilePath("authentication.json");
    auto authenticationJson = JsonFileReader::ReadJson(fileName);
    if (authenticationJson.is_null())
        return;
    auto serviceAccount = JsonExtension::CreateFromJson<Account>(authenticationJson.value("serviceAccount", nlohmann::json()));
    _accounts.push_back(serviceAccount);
    auto deviceAccount = JsonExtension::CreateFromJson<Account>(authenticationJson.value("deviceAccount", nlohmann::json()));
    _accounts.push_back(deviceAccount);
    auto usersJson = authenticationJson.value("users", nlohmann::json::array());
    for (auto& userJson : usersJson)
    {
        auto userAccount = JsonExtension::CreateFromJson<Account>(userJson);
        _accounts.push_back(userAccount);
    }
}

std::string AccountManager::ToBase64(const std::string& str)
{
    std::istringstream incoming(str);
    std::stringstream outgoing;
    base64::encoder encoder;
    encoder.encode(incoming, outgoing);
    outgoing.flush();
    return outgoing.str();
}

std::string AccountManager::FromBase64(const std::string& str)
{
    std::istringstream incoming(str);
    std::stringstream outgoing;
    base64::decoder decoder;
    decoder.decode(incoming, outgoing);
    outgoing.flush();
    return outgoing.str();
}
