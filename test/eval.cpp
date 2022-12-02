#include "doctest.h"
#include <mal.h>

TEST_SUITE("[eval]") {
    TEST_CASE("Numbers only") {
        auto env = std::make_shared<mal::MalEnv>();
        CHECK_EQ(mal::rep(env, "(+ 1 2)"), "3");
        CHECK_EQ(mal::rep(env, "(+ 3 2 6)"), "11");
        CHECK_EQ(mal::rep(env, "(- 5 2)"), "3");
        CHECK_EQ(mal::rep(env, "(- 3 2 6)"), "-5");
        CHECK_EQ(mal::rep(env, "(- 3 2 -6)"), "7");
        CHECK_EQ(mal::rep(env, "(+ 2 (* 3 4))"), "14");
        CHECK_EQ(mal::rep(env, "(- 3)"), "-3");
        CHECK_EQ(mal::rep(env, "(* 4 2)"), "8");
        CHECK_EQ(mal::rep(env, "(* 3 2 6)"), "36");
        CHECK_EQ(mal::rep(env, "(/ 64 2)"), "32");
        CHECK_EQ(mal::rep(env, "(/ 64 2 4)"), "8");
    }
}
