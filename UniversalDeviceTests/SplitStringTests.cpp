#include "catch.hpp"

#include "String.hpp"

TEST_CASE("String splitting") {
    std::vector<std::string> result;

    result = SplitString("a/bb/ccc/", ';');
    REQUIRE(result.size() == 1);
    REQUIRE(result[0] == "a/bb/ccc/");

    result = SplitString("a/bb/ccc/", '/');
    REQUIRE(result.size() == 3);
    REQUIRE(result[0] == "a");
    REQUIRE(result[1] == "bb");
    REQUIRE(result[2] == "ccc");

    result = SplitString("a/bb/ccc/", '/', true);
    REQUIRE(result.size() == 4);
    REQUIRE(result[0] == "a");
    REQUIRE(result[1] == "bb");
    REQUIRE(result[2] == "ccc");
    REQUIRE(result[3].empty());

    result = SplitString("/a/bb/ccc/", '/');
    REQUIRE(result.size() == 3);
    REQUIRE(result[0] == "a");
    REQUIRE(result[1] == "bb");
    REQUIRE(result[2] == "ccc");

    result = SplitString("/a/bb/ccc/", '/', true);
    REQUIRE(result.size() == 5);
    REQUIRE(result[0].empty());
    REQUIRE(result[1] == "a");
    REQUIRE(result[2] == "bb");
    REQUIRE(result[3] == "ccc");
    REQUIRE(result[4].empty());

    result = SplitString("/a/bb//ccc/", '/');
    REQUIRE(result.size() == 3);
    REQUIRE(result[0] == "a");
    REQUIRE(result[1] == "bb");
    REQUIRE(result[2] == "ccc");

    result = SplitString("/a/bb//ccc/", '/', true);
    REQUIRE(result.size() == 6);
    REQUIRE(result[0].empty());
    REQUIRE(result[1] == "a");
    REQUIRE(result[2] == "bb");
    REQUIRE(result[3].empty());
    REQUIRE(result[4] == "ccc");
    REQUIRE(result[5].empty());
}
