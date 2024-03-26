#include "catch.hpp"

#include "Base64Helper.h"


TEST_CASE("To base64")
{
    REQUIRE(Base64Helper::ToBase64("login5:password5") == "bG9naW41OnBhc3N3b3JkNQ==");
}

TEST_CASE("From base64")
{
    REQUIRE(Base64Helper::FromBase64("bG9naW4zOnBhc3N3b3JkMw==") == "login3:password3");
}
