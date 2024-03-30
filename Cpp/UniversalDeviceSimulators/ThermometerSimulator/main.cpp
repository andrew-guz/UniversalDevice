#include <chrono>
#include <iostream>
#include <random>
#include <thread>

#include "Constants.h"
#include "Defines.h"
#include "JsonExtension.h"
#include "Logger.h"
#include "MessageHelper.h"
#include "Parameters.h"
#include "PeriodSettings.h"
#include "RequestHelper.h"
#include "ThermometerCurrentValue.h"
#include "UrlHelper.h"

using namespace std::literals;

std::random_device random_device;
std::mt19937 random_generator(random_device());
std::uniform_int_distribution<> distribution(1, 10);
Parameters parameters;
float temperature;

PeriodSettings GetSettings() {
    auto replyJson = RequestHelper::DoGetRequest({"127.0.0.1", parameters._port, UrlHelper::Url(API_DEVICE_SETTINGS, "<string>", parameters._id.data())}, Constants::LoginDevice);
    if (replyJson.is_null())
        return {};
    return JsonExtension::CreateFromJson<PeriodSettings>(replyJson);
}

void GetTemperature() {
    // generate new value
    auto random_value = distribution(random_generator);
    switch (random_value) {
        case 1:
            temperature += parameters._delta;
            break;
        case 2:
            temperature -= parameters._delta;
            break;
    }
}

void ReportTemperature() {
    ThermometerCurrentValue currentValue;
    currentValue._value = temperature;
    auto message = MessageHelper::Create(Constants::DeviceTypeThermometer, parameters._id, Constants::SubjectThermometerCurrentValue, currentValue.ToJson());
    RequestHelper::DoPostRequest({"127.0.0.1", parameters._port, API_DEVICE_INFORM}, Constants::LoginDevice, message.ToJson());
}

int main(int argc, char* argv[]) {
    Logger::SetLogLevel(LogLevel::INFO);

    parameters = Parameters::ReadFromFile("ThermometerSimulator.json");
    temperature = parameters._startTemperature;

    auto time1 = std::chrono::system_clock::now();
    while (true) {
        std::this_thread::sleep_for(std::chrono::duration(std::chrono::milliseconds(500)));
        auto settings = GetSettings();
        auto time2 = std::chrono::system_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(time2 - time1).count() > settings._period) {
            GetTemperature();
            ReportTemperature();
            time1 = time2;
        }
    }
    return 0;
}
