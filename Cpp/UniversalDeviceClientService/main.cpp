#include "Application.h"

int main(int argc, char **argv)
{
    auto settings = Settings::ReadSettings();

    return Wt::WRun(argc, argv, [&](const Wt::WEnvironment& env)
    {
        return std::make_unique<Application>(settings, env);
    });
}