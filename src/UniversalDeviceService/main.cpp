#include <cstring>
#include <iostream>

#include <fmt/format.h>

#include "Server.hpp"
#include "Version.hpp"

int main(int argc, char** argv) {
    if (argc == 2 && strcmp(argv[1], "--version") == 0) {
        std::cout << fmt::format("{}.{}.{}", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH) << std::endl;
        return 0;
    }

    return Server{}.run();
}
