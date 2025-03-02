

#include "UniversalDeviceSimulator.hpp"

#include <random>
#include <string>

#include "Enums.hpp"
#include "MessageHelper.hpp"
#include "UniversalData.hpp"
#include "UniversalDeviceCurrentValues.hpp"

namespace {
    std::random_device random_device;
    std::mt19937 random_generator(random_device());
    std::uniform_int_distribution<> bool_distribution(0, 1);
    std::uniform_int_distribution<> int_distribution(-10, 10);
    std::uniform_real_distribution<> double_distribution(-10.0, 10.0);
    std::uniform_int_distribution<> string_distribution(97, 122);
} // namespace

UniversalDeviceSimulator::UniversalDeviceSimulator() :
    Simulator(DeviceType::UniversalDevice),
    values{ std::map<std::string, UniversalData>{
        { "param_bool", UniversalData{ false } },
        { "param_int", UniversalData{ 42 } },
        { "param_double", UniversalData{ 42.0 } },
        { "param_string", UniversalData{ std::string{ "test" } } },
    } } {}

UniversalDeviceCurrentValues UniversalDeviceSimulator::GetValues() const { return values; }

int UniversalDeviceSimulator::GetPeriod() const { return periodSettings._period; }

void UniversalDeviceSimulator::SendValues() {
    values._values.at("param_bool") = static_cast<bool>(bool_distribution(random_generator));
    values._values.at("param_int") = values._values.at("param_int").Get<int>() + int_distribution(random_generator);
    values._values.at("param_double") = values._values.at("param_double").Get<double>() + double_distribution(random_generator);
    std::string stringValue;
    for (size_t i = 0; i < 10; ++i) {
        stringValue += static_cast<char>(string_distribution(random_generator));
    }
    values._values.at("param_string") = stringValue;
    auto valuesMessage = MessageHelper::Create(GetType(), GetId(), Subject::UniversalDeviceCurrentState, values);
    SendMessage(valuesMessage);
}

void UniversalDeviceSimulator::OnMessage(const ix::WebSocketMessagePtr& message) {
    if (message->type == ix::WebSocketMessageType::Message) {
        try {
            auto json = nlohmann::json::parse(message->str);
            if (json.contains("period")) {
                periodSettings = json.get<PeriodSettings>();
                SendValues();
            }
        } catch (...) {
        }
    }
}
