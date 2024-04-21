#include "ThermometerSimulator.hpp"

#include <cstdlib>
#include <random>

#include <nlohmann/json.hpp>

#include "JsonExtension.h"

namespace {
    std::random_device random_device;
    std::mt19937 random_generator(random_device());
    std::uniform_int_distribution<> distribution(1, 4);
} // namespace

ThermometerSimulator::ThermometerSimulator() : Simulator("thermometer") {
    parameters = Parameters::ReadFromFile();
    temperature = parameters._startTemperature;
    AskForSettings();
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

void ThermometerSimulator::OnMessage(const ix::WebSocketMessagePtr& message) {
    if (message->type == ix::WebSocketMessageType::Message) {
        try {
            auto json = nlohmann::json::parse(message->str);
            if (json.contains("period")) {
                periodSettings = JsonExtension::CreateFromJson<PeriodSettings>(json);
            }
        } catch (...) {
        }
    }
}
