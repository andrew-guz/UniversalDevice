#include "Application.hpp"
#include "Logger.hpp"

int main(int argc, char** argv) {
    Logger::SetLogLevel(LogLevel::INFO);

    LOG_INFO << "Starting Client service..." << std::endl;

    auto settings = Settings::ReadSettings();

    const auto result = Wt::WRun(argc, argv, [&](const Wt::WEnvironment& env) { return std::make_unique<Application>(settings, env); });

    LOG_INFO << "Stopping Client service" << std::endl;

    return result;
}
