#include <chrono>

#include "Logger.hpp"
#include "UniversalDeviceSimulator.hpp"

int main(int argc, char* argv[]) {
    Logger::SetLogLevel(LogLevel::INFO);

    UniversalDeviceSimulator simulator;

    auto time1 = std::chrono::system_clock::now();
    while (true) {
        std::this_thread::sleep_for(std::chrono::duration(std::chrono::milliseconds(500)));
        auto time2 = std::chrono::system_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(time2 - time1).count() > simulator.GetPeriod()) {
            simulator.SendValues();
            time1 = time2;
        }
    }
    return 0;
}
