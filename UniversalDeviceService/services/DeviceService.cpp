#include "DeviceService.hpp"

#include "AccountManager.hpp"
#include "CurrentTime.hpp"
#include "Defines.hpp"
#include "EventTableStorageCache.hpp"
#include "Logger.hpp"
#include "Marshaling.hpp"
#include "MessageHelper.hpp"
#include "SimpleTableStorageCache.hpp"
#include "StorageCacheFactory.hpp"
#include "TimeHelper.hpp"
#include "WebSocketAuthentication.hpp"
#include "WebsocketsCache.hpp"

void TimerThreadFunction(std::function<void(void)> timerFunction) {
    auto [startHour, startMinute] = TimeHelper::GetHourMinute(std::chrono::system_clock::now());
    auto startValue = startHour * 60 + startMinute;
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        auto [currentHour, currentMinute] = TimeHelper::GetHourMinute(std::chrono::system_clock::now());
        auto currentValue = currentHour * 60 + currentMinute;
        if (currentValue != startValue) {
            timerFunction();
            startValue = currentValue;
        }
    }
}

DeviceService::DeviceService(IQueryExecutor* queryExecutor) : BaseService(queryExecutor) {}

void DeviceService::Initialize(CrowApp& app) {
    CROW_ROUTE(app, API_DEVICE_SETTINGS).methods(crow::HTTPMethod::GET)(BaseService::bind(this, &DeviceService::GetSettings));
    CROW_ROUTE(app, API_DEVICE_SETTINGS).methods(crow::HTTPMethod::POST)([&](const crow::request& request, const std::string& idString) {
        return SetSettings(request, idString);
    });
    CROW_ROUTE(app, API_DEVICE_COMMANDS).methods(crow::HTTPMethod::GET)(BaseService::bind(this, &DeviceService::GetCommands));
    CROW_ROUTE(app, API_DEVICE_COMMANDS).methods(crow::HTTPMethod::POST)([&](const crow::request& request, const std::string& idString) {
        return SetCommands(request, idString);
    });
    CROW_ROUTE(app, API_DEVICE).methods(crow::HTTPMethod::DELETE)(BaseService::bind(this, &DeviceService::DeleteDevice));
    CROW_WEBSOCKET_ROUTE(app, API_DEVICE_WEBSOCKETS)
        .onopen([&](crow::websocket::connection& connection) { LOG_INFO << "Incoming ip - " << connection.get_remote_ip() << "." << std::endl; })
        .onmessage([&](crow::websocket::connection& connection, const std::string& data, bool is_binary) {
            return OnWebSocketMessage(connection, data, is_binary);
        })
        .onclose([&](crow::websocket::connection& connection, const std::string& reason) { return OnWebSocketClose(connection, reason); });

    // also start thread for timer events
    auto timerFunction = std::bind(&DeviceService::TimerFunction, this);
    auto timingThread = new std::thread(TimerThreadFunction, timerFunction);
    timingThread->detach();
}

crow::response DeviceService::GetSettings(const std::string& idString) {
    try {
        auto storageCache = SimpleTableStorageCache::GetSettingsCache(_queryExecutor);
        SimpleTableSelectInput what;
        what._id = idString;
        SimpleTableSelectOutput result;
        auto problem = storageCache->Select(what, result);
        switch (problem._type) {
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
    } catch (...) {
        LOG_ERROR << "Something went wrong in DeviceService::GetSettings." << std::endl;
        return crow::response(crow::BAD_REQUEST);
    }
    return crow::response(crow::OK, std::string());
}

crow::response DeviceService::SetSettings(const crow::request& request, const std::string& idString) {
    try {
        auto settingsString = request.body;
        auto storageCache = SimpleTableStorageCache::GetSettingsCache(_queryExecutor);
        SimpleTableInsertOrReplaceInput what;
        what._id = idString;
        what._data = settingsString;
        auto problem = storageCache->InsertOrReplace(what);
        switch (problem._type) {
            case StorageCacheProblemType::NoProblems: {
                auto connection = WebsocketsCache::Instance()->GetWebSocketConnection(Uuid(idString));
                if (connection)
                    connection->send_text(settingsString);
                return crow::response(crow::OK);
            } break;
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
    } catch (...) {
        LOG_ERROR << "Something went wrong in DeviceService::SetSettings." << std::endl;
    }
    return crow::response(crow::BAD_REQUEST);
}

crow::response DeviceService::GetCommands(const std::string& idString) {
    try {
        auto storageCache = SimpleTableStorageCache::GetCommandsCache(_queryExecutor);
        SimpleTableSelectInput what;
        what._id = idString;
        SimpleTableSelectOutput result;
        auto problem = storageCache->Select(what, result);
        switch (problem._type) {
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
    } catch (...) {
        LOG_ERROR << "Something went wrong in DeviceService::GetCommands." << std::endl;
        return crow::response(crow::BAD_REQUEST);
    }
    return crow::response(crow::OK, std::string());
}

crow::response DeviceService::SetCommands(const crow::request& request, const std::string& idString) {
    try {
        auto commandsString = request.body;
        auto storageCache = SimpleTableStorageCache::GetCommandsCache(_queryExecutor);
        SimpleTableInsertOrReplaceInput what;
        what._id = idString;
        what._data = commandsString;
        auto problem = storageCache->InsertOrReplace(what);
        switch (problem._type) {
            case StorageCacheProblemType::NoProblems: {
                auto connection = WebsocketsCache::Instance()->GetWebSocketConnection(Uuid(idString));
                if (connection)
                    connection->send_text(commandsString);
                return crow::response(crow::OK);
            } break;
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
    } catch (...) {
        LOG_ERROR << "Something went wrong in DeviceService::SetCommands." << std::endl;
    }
    return crow::response(crow::BAD_REQUEST);
}

crow::response DeviceService::DeleteDevice(const std::string& idString) {
    try {
        // delete device from all tables
        for (const auto& table : _queryExecutor->GetDeviceRelatedTables()) {
            std::stringstream queryStream;
            queryStream << "DELETE FROM " << table << " WHERE id = '" << idString << "'";
            queryStream.flush();
            if (!_queryExecutor->Delete(queryStream.str())) {
                LOG_ERROR << "Failed to delete device " << idString << " from " << table << std::endl;
                return crow::response(crow::BAD_REQUEST);
            }
        }
        {
            SimpleTableDeleteInput what;
            what._id = idString;
            if (SimpleTableStorageCache::GetSettingsCache(_queryExecutor)->Delete(what)._type != StorageCacheProblemType::NoProblems) {

                LOG_ERROR << "Failed to delete device " << idString << " from Settings" << std::endl;
                return crow::response(crow::BAD_REQUEST);
            }
            if (SimpleTableStorageCache::GetCommandsCache(_queryExecutor)->Delete(what)._type != StorageCacheProblemType::NoProblems) {
                LOG_ERROR << "Failed to delete device " << idString << " from Commands" << std::endl;
                return crow::response(crow::BAD_REQUEST);
            }
        }
        // TODO: Clean event table. Right now deleted id is not a column of event table...
        return crow::response{ crow::status::OK };
    } catch (...) {
        LOG_ERROR << "Something went wrong in DeviceService::DeleteDevice." << std::endl;
    }
    return crow::response(crow::BAD_REQUEST);
}

void DeviceService::OnWebSocketMessage(crow::websocket::connection& connection, const std::string& data, bool is_binary) {
    if (is_binary)
        return;
    try {
        auto timestamp = std::chrono::system_clock::now();
        auto message = BaseServiceExtension::GetMessageFromWebSocketData(data);
        if (message._header._subject == Subject::WebSocketAuthorization) {
            auto webSocketAuthentication = message._data.get<WebSocketAuthentication>();
            if (AccountManager::Instance()->IsValidUser(webSocketAuthentication._authString))
                WebsocketsCache::Instance()->AddWebSocketConnection(message._header._description._id, connection);
            else {
                LOG_ERROR << "Not authorized connection." << std::endl;
                // ask for authorization again
                connection.send_text("{ \"reauthorize\" : true }");
            }
        } else {
            auto knownConnection = WebsocketsCache::Instance()->GetWebSocketConnection(message._header._description._id);
            if (knownConnection) {
                // not all messages need answers
                auto result = CallProcessorsJsonResult(timestamp, message);
                if (!result.is_null())
                    connection.send_text(result.dump());
            } else {
                LOG_ERROR << "Not authorized connection." << std::endl;
                // ask for authorization again
                connection.send_text("{ \"reauthorize\" : true }");
            }
        }
    } catch (...) {
        LOG_ERROR << "Something went wrong in DeviceService::OnWebSocketMessage." << std::endl;
    }
}

void DeviceService::OnWebSocketClose(crow::websocket::connection& connection, const std::string& reason) {
    WebsocketsCache::Instance()->DeleteWebSocketConnection(connection);
}

void DeviceService::TimerFunction() {
    CurrentTime currentTime;
    currentTime._timestamp = std::chrono::system_clock::now();
    auto message = MessageHelper::Create(DeviceType::Timer, Constants::PredefinedIdTimer, Subject::TimerEvent, currentTime);
    CallProcessorsNoResult(std::chrono::system_clock::now(), message);
}
