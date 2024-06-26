#include "Application.hpp"
#include "Logger.hpp"

int main(int argc, char** argv) {
    Logger::SetLogLevel(LogLevel::INFO);

    auto settings = Settings::ReadSettings();

    return Wt::WRun(argc, argv, [&](const Wt::WEnvironment& env) { return std::make_unique<Application>(settings, env); });
}
