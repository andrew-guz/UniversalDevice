#include "DeviceService.h"

#include "Defines.h"
#include "Logger.h"
#include "TimeHelper.h"
#include "CurrentTime.h"
#include "MessageHelper.h"

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
        std::stringstream queryStream;
        queryStream << "SELECT settings FROM Settings WHERE id = '"
            << idString
            << "'";
        queryStream.flush();
        std::vector<std::vector<std::string>> data;
        if (_queryExecutor->Select(queryStream.str(), data))
        {
            if (data.size() == 1)
            {
                auto settingsString = data[0][1];
                if (!settingsString.empty())
                    return crow::response(crow::OK, settingsString);
                else
                    LOG_INFO << "Empty settings for device " << idString << "." << std::endl;
            }
            else
            {
                if (data.size() == 0)
                    LOG_DEBUG << "No settings for device " << idString << "." << std::endl;
                else
                    LOG_ERROR << "Too many settings for device " << idString << "." << std::endl;
            }
        }
        else
            LOG_SQL_ERROR(queryStream.str());
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
        if (!settingsString.empty())
        {
            std::stringstream queryStream;
            queryStream << "INSERT OR REPLACE INTO Settings (id, settings) VALUES ('"
                << idString
                << "', '"
                << settingsString
                << "')";
            queryStream.flush();
            if (_queryExecutor->Execute(queryStream.str()))
                return crow::response(crow::OK);
            else
                LOG_SQL_ERROR(queryStream.str());
        }
        else
            LOG_ERROR << "Invalid settings " << settingsString << "." << std::endl;    
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
        std::stringstream queryStream;
        queryStream << "SELECT commands FROM Commands WHERE id = '"
            << idString
            << "'";
        queryStream.flush();
        std::vector<std::vector<std::string>> data;
        if (_queryExecutor->Select(queryStream.str(), data))
        {
            if (data.size() == 1)
            {
                auto commandsString = data[0][1];
                if (!commandsString.empty())
                    return crow::response(crow::OK, commandsString);
                else
                    LOG_INFO << "Empty commands for device " << idString << "." << std::endl;
            }
            else
            {
                if (data.size() == 0)
                    LOG_DEBUG << "No commands for device " << idString << "." << std::endl;
                else
                    LOG_ERROR << "Too many commands for device " << idString << "." << std::endl;
            }
        }
        else
            LOG_SQL_ERROR(queryStream.str());
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
        if (!commandsString.empty())
        {
            std::stringstream queryStream;
            queryStream << "INSERT OR REPLACE INTO Commands (id, commands) VALUES ('"
                << idString
                << "', '"
                << commandsString
                << "')";
            queryStream.flush();
            if (_queryExecutor->Execute(queryStream.str()))
                return crow::response(crow::OK);
            else
                LOG_SQL_ERROR(queryStream.str());
        }
        else
            LOG_ERROR << "Invalid commands " << commandsString << "." << std::endl;    
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

void DeviceService::TimerFunction()
{
    CurrentTime currentTime;
    currentTime._timestamp = std::chrono::system_clock::now();
    auto message = MessageHelper::Create(Constants::DeviceTypeTimer, Constants::PredefinedIdTimer, Constants::SubjectTimerEvent, currentTime.ToJson());
    CallProcessorsNoResult(std::chrono::system_clock::now(), message);
}
