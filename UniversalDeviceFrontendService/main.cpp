#include <exception>

#include <fmt/format.h>

#include "Application.hpp"
#include "Logger.hpp"

int main(int argc, char** argv) {
    int result = -1;

    try {
        Logger::SetLogLevel(LogLevel::INFO);

        LOG_INFO_MSG("Starting Client service...");

        auto settings = Settings::ReadSettings();

        result = Wt::WRun(argc, argv, [&](const Wt::WEnvironment& env) { return std::make_unique<Application>(settings, env); });

        LOG_INFO_MSG("Stopping Client service");
    } catch (const std::exception& ex) {
        LOG_ERROR_MSG(fmt::format("Exception caught: '{}'", ex.what()));
    } catch (...) {
        LOG_ERROR_MSG("Unknown exception caught");
    }

    return result;
}
