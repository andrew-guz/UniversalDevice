#include "ThermometerSimulator.hpp"

#include <random>

#include <ixwebsocket/IXWebSocketMessage.h>
#include <ixwebsocket/IXWebSocketMessageType.h>
#include <nlohmann/json_fwd.hpp>

#include "Enums.hpp"
#include "MessageHelper.hpp"
#include "Parameters.hpp"
#include "PeriodSettings.hpp"
#include "Simulator.hpp"
#include "ThermometerValue.hpp"

namespace {
    std::random_device random_device;
    std::mt19937 random_generator(random_device());
    std::uniform_int_distribution<> distribution(1, 4);
} // namespace

ThermometerSimulator::ThermometerSimulator() :
    Simulator(DeviceType::Thermometer) {
    parameters = Parameters::ReadFromFile();
    temperature = parameters._startTemperature;
}

float ThermometerSimulator::GetTemperature() {
    // generate new value
    auto random_value = distribution(random_generator);
    switch (random_value) {
        case 1:
            temperature += parameters._delta;
            break;
        case 2:
            temperature -= parameters._delta;
            break;
        default:
            break;
    }
    return temperature;
}

int ThermometerSimulator::GetPeriod() const { return periodSettings._period; }

void ThermometerSimulator::SendTemperature() {
    ThermometerValue temperatureValue;
    temperatureValue._value = GetTemperature();
    auto temperatureMessage = MessageHelper::CreateDeviceMessage(GetType(), GetId(), Subject::ThermometerCurrentValue, temperatureValue);
    SendMessage(temperatureMessage);
}

void ThermometerSimulator::OnMessage(const ix::WebSocketMessagePtr& message) {
    if (message->type == ix::WebSocketMessageType::Message) {
        try {
            auto json = nlohmann::json::parse(message->str);
            if (json.contains("period")) {
                periodSettings = json.get<PeriodSettings>();
                SendTemperature();
            }
        } catch (...) {
        }
    }
}
