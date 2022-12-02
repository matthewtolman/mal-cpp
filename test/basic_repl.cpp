#include "doctest.h"
#include <mal.h>

static auto rp(std::string s, bool readable_print = true)-> std::string {
    return mal::PRINT(mal::READ(std::move(s)), readable_print);
}

TEST_SUITE("[repl][basic]") {
    TEST_CASE("Numbers") {
        CHECK_EQ(rp("123"), "123");
        CHECK_EQ(rp("123  "), "123");
        CHECK_EQ(rp("   123  "), "123");
    }

    TEST_CASE("Booleans") {
        CHECK_EQ(rp("true"), "true");
        CHECK_EQ(rp("false  "), "false");
    }

    TEST_CASE("Symbols") {
        CHECK_EQ(rp("abc"), "abc");
        CHECK_EQ(rp("abc  "), "abc");
        CHECK_EQ(rp("   abc  "), "abc");
    }

    TEST_CASE("Comment") {
        CHECK_EQ(rp("; abc"), rp("; abc 123"));
    }

    TEST_CASE("Lists") {
        CHECK_EQ(rp("(123 456)"), "(123 456)");
        CHECK_EQ(rp("( 123 456 789 ) "), "(123 456 789)");
        CHECK_EQ(rp("( + 2 (* 3 4) ) "), "(+ 2 (* 3 4))");
        CHECK_EQ(rp("(123 ;abc\n 456)"), "(123 456)");
    }

    TEST_CASE("Maps") {
        CHECK_EQ(rp("{123 456}"), "{123 456}");
        CHECK_EQ(rp("{ 123 456 789 abc} "), "{123 456 789 abc}");
        CHECK_EQ(rp("{ 123 456 ;abc\n 789 abc} "), "{123 456 789 abc}");
    }

    TEST_CASE("Vectors") {
        CHECK_EQ(rp("[123 456]"), "[123 456]");
        CHECK_EQ(rp("( 123 456 789 ) "), "(123 456 789)");
        CHECK_EQ(rp("( + 2 (* 3 4) ) "), "(+ 2 (* 3 4))");
        CHECK_EQ(rp("[123 ;abc\n 456]"), "[123 456]");
    }

    TEST_CASE("Strings") {
        CHECK_EQ(rp(R"("hello world")", true), R"("hello world")");
        CHECK_EQ(rp(R"("hello\n \"world\" \\c")", true), R"("hello\n \"world\" \\c")");
        CHECK_EQ(rp(R"("hello world")", false), R"(hello world)");
        CHECK_EQ(rp(R"("hello\n \"world\" \\c")", false), R"(hello
 "world" \c)");
    }

    TEST_CASE("Keywords") {
        CHECK_EQ(rp(":abc"), ":abc");
        CHECK_EQ(rp(":abc  "), ":abc");
        CHECK_EQ(rp("   :abc  "), ":abc");
    }

    TEST_CASE("Incomplete List parsing fails") {
        CHECK_THROWS_AS(rp("(1 2 3"), std::runtime_error);
    }

    TEST_CASE("Incomplete Vector parsing fails") {
        CHECK_THROWS_AS(rp("[1 2 3"), std::runtime_error);
    }

    TEST_CASE("Invalid Map parsing fails") {
        CHECK_THROWS_AS(rp("{1 2 3 4"), std::runtime_error);
        CHECK_THROWS_AS(rp("{1 2 3"), std::runtime_error);
        CHECK_THROWS_AS(rp("{1 2 3}"), std::runtime_error);
    }
}