#include <exception>

#include <fmt/format.h>

#include "Application.hpp"
#include "Logger.hpp"
#include "Version.hpp"

int main(int argc, char** argv) {
    if (argc == 2 && strcmp(argv[1], "--version") == 0) {
        std::cout << fmt::format("{}.{}.{}", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH) << std::endl;
        return 0;
    }

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
