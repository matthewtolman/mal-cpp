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

    TEST_CASE("read-string") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((read-string "5"))"), "5");
        REQUIRE_EQ(mal::rep(env, R"((read-string "\"hello\""))"), R"("hello")");
    }

    TEST_CASE("slurp") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((slurp "test.txt"))"), R"("Hello World!\nHow are you?")");
    }

    TEST_CASE("list") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((list 1 2 3 "hello"))"), R"((1 2 3 "hello"))");
    }

    TEST_CASE("list?") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((list? 1))"), R"(false)");
        REQUIRE_EQ(mal::rep(env, R"((list? (list 1 2 3)))"), R"(true)");
    }

    TEST_CASE("empty?") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((empty? (list)))"), R"(true)");
        REQUIRE_EQ(mal::rep(env, R"((empty? (list 1 2 3)))"), R"(false)");
        REQUIRE_EQ(mal::rep(env, R"((empty? []))"), R"(true)");
        REQUIRE_EQ(mal::rep(env, R"((empty? [1 2 3]))"), R"(false)");
        REQUIRE_EQ(mal::rep(env, R"((empty? {}))"), R"(true)");
        REQUIRE_EQ(mal::rep(env, R"((empty? {1 2 3 4}))"), R"(false)");
    }

    TEST_CASE("count") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((count (list)))"), R"(0)");
        REQUIRE_EQ(mal::rep(env, R"((count (list 1 2 3)))"), R"(3)");
        REQUIRE_EQ(mal::rep(env, R"((count []))"), R"(0)");
        REQUIRE_EQ(mal::rep(env, R"((count [1 2 3]))"), R"(3)");
        REQUIRE_EQ(mal::rep(env, R"((count {}))"), R"(0)");
        REQUIRE_EQ(mal::rep(env, R"((count {1 2 3 4}))"), R"(2)");
    }

    TEST_CASE("=") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((= 5 5))"), R"(true)");
        REQUIRE_EQ(mal::rep(env, R"((= 5 5 5 5 5 5))"), R"(true)");
        REQUIRE_EQ(mal::rep(env, R"((= 5 5 15 5 5 5))"), R"(false)");
    }

    TEST_CASE("<") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((< 5 6))"), R"(true)");
        REQUIRE_EQ(mal::rep(env, R"((< 5 6 7 8 9 10))"), R"(true)");
        REQUIRE_EQ(mal::rep(env, R"((< 5 6 15 8 9 10))"), R"(false)");
        REQUIRE_EQ(mal::rep(env, R"((< 5 5))"), R"(false)");
    }

    TEST_CASE("<=") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((<= 5 6))"), R"(true)");
        REQUIRE_EQ(mal::rep(env, R"((<= 5 6 7 8 9 9))"), R"(true)");
        REQUIRE_EQ(mal::rep(env, R"((<= 5 6 15 8 9 10))"), R"(false)");
    }

    TEST_CASE(">") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((> 6 5))"), R"(true)");
        REQUIRE_EQ(mal::rep(env, R"((> 6 5 4 3 2 1))"), R"(true)");
        REQUIRE_EQ(mal::rep(env, R"((> 6 5 15 8 9 10))"), R"(false)");
        REQUIRE_EQ(mal::rep(env, R"((> 5 5))"), R"(false)");
    }

    TEST_CASE(">=") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((>= 8 6))"), R"(true)");
        REQUIRE_EQ(mal::rep(env, R"((>= 5 5 4 3 2 1))"), R"(true)");
        REQUIRE_EQ(mal::rep(env, R"((>= 5 6 15))"), R"(false)");
    }

    TEST_CASE("<=>") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((<=> 8 6))"), R"(1)");
        REQUIRE_EQ(mal::rep(env, R"((<=> 5 5))"), R"(0)");
        REQUIRE_EQ(mal::rep(env, R"((<=> 5 6))"), R"(-1)");
    }

    TEST_CASE("eval") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((let* [mal-prog (list + 1 2)] (eval mal-prog)))"), R"(3)");
    }

    TEST_CASE("load-file") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((do (load-file "test.lsp") (inc4 3)))"), R"(7)");
    }
}
