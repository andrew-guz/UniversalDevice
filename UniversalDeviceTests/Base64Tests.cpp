#include <catch2/catch_all.hpp>

#include "Base64Helper.hpp"

#include <fstream>
#include <sstream>

TEST_CASE("To base64") { REQUIRE(Base64Helper::ToBase64("login5:password5") == "bG9naW41OnBhc3N3b3JkNQ=="); }

TEST_CASE("From base64") { REQUIRE(Base64Helper::FromBase64("bG9naW4zOnBhc3N3b3JkMw==") == "login3:password3"); }

void TestLogFile(const std::string_view fileName) {
    std::ifstream logStream(fileName.data());
    std::ostringstream logBuffer;
    logBuffer << logStream.rdbuf();
    auto originalText = logBuffer.str();
    auto base64 = Base64Helper::ToBase64(originalText);
    auto text = Base64Helper::FromBase64(base64);
    REQUIRE(text == originalText);
}

TEST_CASE("Log files content") {
    TestLogFile("testLog1.log");
    TestLogFile("testLog2.log");
}
