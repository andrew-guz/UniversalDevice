#include <iostream>
#include <thread>
#include <chrono>
#include <random>

#include "Logger.h"
#include "Parameters.h"
#include "Constants.h"
#include "Defines.h"
#include "RelaySettings.h"
#include "RelayCurrentState.h"
#include "MessageHelper.h"
#include "UrlHelper.h"
#include "RequestHelper.h"

using namespace std::literals;

int state = 0;
Parameters parameters;

RelaySettings GetSettings()
{
    auto replyJson = RequestHelper::DoGetRequest({ "127.0.0.1", parameters._port, UrlHelper::Url(API_DEVICE_SETTINGS, "<string>", parameters._id.data()) }, Constants::LoginDevice);
    if (replyJson.is_null())
        return {};
    return JsonExtension::CreateFromJson<RelaySettings>(replyJson);
}

RelayCurrentState GetCommands()
{
    auto replyJson = RequestHelper::DoGetRequest({ "127.0.0.1", parameters._port, UrlHelper::Url(API_DEVICE_COMMANDS, "<string>", parameters._id.data()) }, Constants::LoginDevice);
    if (replyJson.is_null())
        return {};
    return JsonExtension::CreateFromJson<RelayCurrentState>(replyJson);
}

void ReportState()
{
    RelayCurrentState currentState;
    currentState._state = state;
    auto message = MessageHelper::Create(Constants::DeviceTypeRelay, parameters._id, Constants::SubjectRelayCurrentState, currentState.ToJson());
    RequestHelper::DoPostRequest({"127.0.0.1", parameters._port, API_DEVICE_INFORM}, Constants::LoginDevice, message.ToJson());
}

int main(int argc, char* argv[])
{
    Logger::SetLogLevel(LogLevel::INFO);

    parameters = Parameters::ReadFromFile("RelaySimulator.json");    

    RelaySettings settings;

    auto time1 = std::chrono::system_clock::now();
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::duration(std::chrono::milliseconds(500)));
        settings = GetSettings();
        auto command = GetCommands();
        auto newState = command._state;
        if (newState != state)
        {
            state = newState;
            ReportState();
        }
        auto time2 = std::chrono::system_clock::now();
        if(std::chrono::duration_cast<std::chrono::milliseconds>(time2 - time1).count() > settings._period)
        {
            ReportState();
            time1 = time2;
        }
    }
    return 0;
}
