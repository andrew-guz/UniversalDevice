#include <exception>

#include "Application.hpp"
#include "Logger.hpp"

int main(int argc, char** argv) {
    int result = -1;

    try {
        Logger::SetLogLevel(LogLevel::INFO);

        LOG_INFO << "Starting Client service..." << std::endl;

        auto settings = Settings::ReadSettings();

        result = Wt::WRun(argc, argv, [&](const Wt::WEnvironment& env) { return std::make_unique<Application>(settings, env); });

        LOG_INFO << "Stopping Client service" << std::endl;
    } catch (const std::exception& ex) {
        LOG_ERROR << "Exception caught: '" << ex.what() << "'" << std::endl;
    } catch (...) {
        LOG_ERROR << "Unknown exception caught" << std::endl;
    }

    return result;
}
