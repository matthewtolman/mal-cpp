#include "doctest.h"
#include <mal.h>

TEST_SUITE("[mal_env][impl]") {
    TEST_CASE("def!") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(def! a 6)"), "6");
        REQUIRE_EQ(mal::rep(env, "a"), "6");
        REQUIRE_EQ(mal::rep(env, "(def! b (+ a 2))"), "8");
        REQUIRE_EQ(mal::rep(env, "(+ a b)"), "14");
    }

    TEST_CASE("let*") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(let* (c 2) c)"), "2");
        REQUIRE_EQ(mal::rep(env, "(let* (c 2) (let* [d 3] (let* {a 5} (+ c d a))))"), "10");
    }

    TEST_CASE("do") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(do (def! b 3) (let* (c 2) (+ c b)))"), "5");
    }

    TEST_CASE("fn*") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(fn* (a) a)"), "(fn* [a] a)");
        REQUIRE_EQ(mal::rep(env, "(fn* [a] a b)"), "(fn* [a] a b)");
        REQUIRE_EQ(mal::rep(env, "((fn* (a) (+ a a)) 5)"), "10");
        REQUIRE_EQ(mal::rep(env, "( (fn* (a) a) 7)"), "7");
        REQUIRE_EQ(mal::rep(env, "( (fn* (a) (+ a 1)) 10)"), "11");
        REQUIRE_EQ(mal::rep(env, "( (fn* (a b) (+ a b)) 2 3)"), "5");
    }

    TEST_CASE("if") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(if true 5 3)"), "5");
        REQUIRE_EQ(mal::rep(env, "(if 12 5 3)"), "5");
        REQUIRE_EQ(mal::rep(env, "(if 0 5 3)"), "5");
        REQUIRE_EQ(mal::rep(env, "(if false 5 3)"), "3");
        REQUIRE_EQ(mal::rep(env, "(if nil 5 3)"), "3");
    }

    TEST_CASE("prn-str") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((pr-str true 5 3 "hello"))"), R"("true 5 3 \"hello\"")");
    }

    TEST_CASE("str") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((str true 5 3 "hello"))"), R"("true53hello")");
    }

    TEST_CASE("prn") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((prn true 5 3 "hello"))"), "nil");
    }

    TEST_CASE("println") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((println true 5 3 "hello"))"), "nil");
    }
}
