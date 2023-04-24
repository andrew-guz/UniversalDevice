#include "DeviceService.h"

#include "Defines.h"
#include "Logger.h"
#include "TimeHelper.h"
#include "CurrentTime.h"
#include "MessageHelper.h"
#include "StorageCacheFactory.h"
#include "SimpleTableStorageCache.h"
#include "JsonExtension.h"
#include "WebSocketAuthentication.h"
#include "WebsocketsCache.h"

void TimerThreadFunction(std::function<void(void)> timerFunction)
{ 
    auto [startHour, startMinute] = TimeHelper::GetHourMinute(std::chrono::system_clock::now());
    auto startValue = startHour * 60 + startMinute;
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        auto [currentHour, currentMinute] = TimeHelper::GetHourMinute(std::chrono::system_clock::now());
        auto currentValue = currentHour * 60 + currentMinute;
        if (currentValue != startValue)
        {
            timerFunction();
            startValue = currentValue;
        }
    }
}

DeviceService::DeviceService(IQueryExecutor* queryExecutor) :
    BaseService(queryExecutor)
{

}

void DeviceService::Initialize(crow::SimpleApp& app)
{
    CROW_ROUTE(app, API_DEVICE_SETTINGS).methods(crow::HTTPMethod::GET)([&](const crow::request& request, const std::string& idString){ return GetSettings(request, idString); });
    CROW_ROUTE(app, API_DEVICE_SETTINGS).methods(crow::HTTPMethod::POST)([&](const crow::request& request, const std::string& idString){ return SetSettings(request, idString); });
    CROW_ROUTE(app, API_DEVICE_COMMANDS).methods(crow::HTTPMethod::GET)([&](const crow::request& request, const std::string& idString){ return GetCommands(request, idString); });
    CROW_ROUTE(app, API_DEVICE_COMMANDS).methods(crow::HTTPMethod::POST)([&](const crow::request& request, const std::string& idString){ return SetCommands(request, idString); });
    CROW_ROUTE(app, API_DEVICE_INFORM).methods(crow::HTTPMethod::POST)([&](const crow::request& request){ return Inform(request); });
    CROW_WEBSOCKET_ROUTE(app, API_DEVICE_WEBSOCKETS)
        .onmessage([&](crow::websocket::connection& connection, const std::string& data, bool is_binary){ return OnWebSocketMessage(connection, data, is_binary); })
        .onclose([&](crow::websocket::connection& connection, const std::string& reason){ return OnWebSocketClose(connection, reason); });

    //also start thread for timer events
    auto timerFunction = std::bind(&DeviceService::TimerFunction, this);
    auto timingThread = new std::thread(TimerThreadFunction, timerFunction);
    timingThread->detach();
}

crow::response DeviceService::GetSettings(const crow::request& request, const std::string& idString)
{
    if (!IsValidUser(request))
        return crow::response(crow::UNAUTHORIZED);
    try
    {
        auto storageCache = StorageCacheFactory::Instance()->GetStorageCache<SimpleTableStorageCache>(_queryExecutor, "Settings", "settings");
        SimpleTableSelectInput what;
        what._id = idString;
        SimpleTableSelectOutput result;
        auto problem = storageCache->Select(what, result);
        switch(problem._type)
        {
        case StorageCacheProblemType::NoProblems:
            return crow::response(crow::OK, result._data);
            break;
        case StorageCacheProblemType::Empty:
            LOG_INFO << "Empty settings for device " << idString << "." << std::endl;
            break;
        case StorageCacheProblemType::NotExists:
            LOG_DEBUG << "No settings for device " << idString << "." << std::endl;
            break;
        case StorageCacheProblemType::TooMany:
            LOG_ERROR << "Too many settings for device " << idString << "." << std::endl;
            break;
        case StorageCacheProblemType::SQLError:
            LOG_SQL_ERROR(problem._message);
            break;
        }
    }
    catch(...)
    {
        LOG_ERROR << "Something went wrong in DeviceService::GetSettings." << std::endl;
        return crow::response(crow::BAD_REQUEST);
    }
    return crow::response(crow::OK, std::string());
}

crow::response DeviceService::SetSettings(const crow::request& request, const std::string& idString)
{
    if (!IsValidUser(request))
        return crow::response(crow::UNAUTHORIZED);
    try
    {
        auto settingsString = request.body;
        auto storageCache = StorageCacheFactory::Instance()->GetStorageCache<SimpleTableStorageCache>(_queryExecutor, "Settings", "settings");
        SimpleTableInsertOrReplaceInput what;
        what._id = idString;
        what._data = settingsString;
        auto problem = storageCache->InsertOrReplace(what);
        switch(problem._type)
        {
        case StorageCacheProblemType::NoProblems:
            {
                auto connection = WebsocketsCache::Instance()->GetWebSocketConnection(Uuid(idString));
                if (connection)
                    connection->send_text(settingsString);
                return crow::response(crow::OK);
            }
            break;
        case StorageCacheProblemType::Empty:
            LOG_ERROR << "Invalid settings " << settingsString << "." << std::endl;
            break;
        case StorageCacheProblemType::NotExists:
            break;
        case StorageCacheProblemType::TooMany:
            break;
        case StorageCacheProblemType::SQLError:
            LOG_SQL_ERROR(problem._message);
            break;
        }        
    }
    catch(...)
    {
        LOG_ERROR << "Something went wrong in DeviceService::SetSettings." << std::endl;
    } 
    return crow::response(crow::BAD_REQUEST);
}

crow::response DeviceService::GetCommands(const crow::request& request, const std::string& idString)
{
    if (!IsValidUser(request))
        return crow::response(crow::UNAUTHORIZED);
    try
    {
        auto storageCache = StorageCacheFactory::Instance()->GetStorageCache<SimpleTableStorageCache>(_queryExecutor, "Commands", "commands");
        SimpleTableSelectInput what;
        what._id = idString;
        SimpleTableSelectOutput result;
        auto problem = storageCache->Select(what, result);
        switch(problem._type)
        {
        case StorageCacheProblemType::NoProblems:
            return crow::response(crow::OK, result._data);
            break;
        case StorageCacheProblemType::Empty:
            LOG_INFO << "Empty commands for device " << idString << "." << std::endl;
            break;
        case StorageCacheProblemType::NotExists:
            LOG_DEBUG << "No commands for device " << idString << "." << std::endl;
            break;
        case StorageCacheProblemType::TooMany:
            LOG_ERROR << "Too many commands for device " << idString << "." << std::endl;
            break;
        case StorageCacheProblemType::SQLError:
            LOG_SQL_ERROR(problem._message);
            break;
        }
    }
    catch(...)
    {
        LOG_ERROR << "Something went wrong in DeviceService::GetCommands." << std::endl;
        return crow::response(crow::BAD_REQUEST);
    }
    return crow::response(crow::OK, std::string());
}

crow::response DeviceService::SetCommands(const crow::request& request, const std::string& idString)
{
    if (!IsValidUser(request))
        return crow::response(crow::UNAUTHORIZED);
    try
    {
        auto commandsString = request.body;
        auto storageCache = StorageCacheFactory::Instance()->GetStorageCache<SimpleTableStorageCache>(_queryExecutor, "Commands", "commands");
        SimpleTableInsertOrReplaceInput what;
        what._id = idString;
        what._data = commandsString;
        auto problem = storageCache->InsertOrReplace(what);
        switch(problem._type)
        {
        case StorageCacheProblemType::NoProblems:
            {
                auto connection = WebsocketsCache::Instance()->GetWebSocketConnection(Uuid(idString));
                if (connection)
                    connection->send_text(commandsString);
                return crow::response(crow::OK);
            }
            break;
        case StorageCacheProblemType::Empty:
            LOG_ERROR << "Invalid commands " << commandsString << "." << std::endl;
            break;
        case StorageCacheProblemType::NotExists:
            break;
        case StorageCacheProblemType::TooMany:
            break;
        case StorageCacheProblemType::SQLError:
            LOG_SQL_ERROR(problem._message);
            break;
        }
    }
    catch(...)
    {
        LOG_ERROR << "Something went wrong in DeviceService::SetCommands." << std::endl;
    } 
    return crow::response(crow::BAD_REQUEST);
}

crow::response DeviceService::Inform(const crow::request& request)
{
    if (!IsValidUser(request))
        return crow::response(crow::UNAUTHORIZED);
    try
    {
        auto timestamp = std::chrono::system_clock::now();
        auto message = BaseServiceExtension::GetMessageFromRequest(request);
        CallProcessorsNoResult(timestamp, message);
        return crow::response(crow::OK);
    }
    catch(...)
    {
        LOG_ERROR << "Something went wrong in DeviceService::Inform." << std::endl;
    }
    return crow::response(crow::BAD_REQUEST);
}

void DeviceService::OnWebSocketMessage(crow::websocket::connection& connection, const std::string& data, bool is_binary)
{
    if (is_binary)
        return;
    try
    {
        auto timestamp = std::chrono::system_clock::now();
        auto message = BaseServiceExtension::GetMessageFromWebSocketData(data);
        if (message._header._subject == Constants::SubjectWebSocketAuthorization)
        {
            auto webSocketAuthentication = JsonExtension::CreateFromJson<WebSocketAuthentication>(message._data);
            if (IsValidUser(webSocketAuthentication._authString))
                WebsocketsCache::Instance()->AddWebSocketConnection(message._header._description._id, connection);
        }
        else
        {
            auto knownConnection = WebsocketsCache::Instance()->GetWebSocketConnection(message._header._description._id);
            if (knownConnection)
            {
                //not all messages need answers
                auto result = CallProcessorsJsonResult(timestamp, message);
                if (!result.is_null())
                    connection.send_text(result.dump());
            }
            else
                LOG_ERROR << "Not authorized connection." << std::endl;
        }
    }
    catch(...)
    {
        LOG_ERROR << "Something went wrong in DeviceService::OnWebSocketMessage." << std::endl;
    }
}

void DeviceService::OnWebSocketClose(crow::websocket::connection& connection, const std::string& reason)
{
    WebsocketsCache::Instance()->DeleteWebSocketConnection(connection);
}

void DeviceService::TimerFunction()
{
    CurrentTime currentTime;
    currentTime._timestamp = std::chrono::system_clock::now();
    auto message = MessageHelper::Create(Constants::DeviceTypeTimer, Constants::PredefinedIdTimer, Constants::SubjectTimerEvent, currentTime.ToJson());
    CallProcessorsNoResult(std::chrono::system_clock::now(), message);
}
