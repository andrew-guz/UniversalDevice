#include <chrono>

#include "Logger.h"
#include "MessageHelper.h"
#include "ThermometerCurrentValue.h"
#include "ThermometerSimulator.hpp"

int main(int argc, char* argv[]) {
    Logger::SetLogLevel(LogLevel::INFO);

    ThermometerSimulator simulator;

    auto time1 = std::chrono::system_clock::now();
    while (true) {
        std::this_thread::sleep_for(std::chrono::duration(std::chrono::milliseconds(500)));
        simulator.AskForSettings();
        auto time2 = std::chrono::system_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(time2 - time1).count() > simulator.GetPeriod()) {
            ThermometerCurrentValue temeratureValue;
            temeratureValue._value = simulator.GetTemperature();
            auto temperatureMessage = MessageHelper::Create(simulator.GetType(), simulator.GetId(), Constants::SubjectThermometerCurrentValue, temeratureValue);
            simulator.SendMessage(temperatureMessage);
            time1 = time2;
        }
    }
    return 0;
}
