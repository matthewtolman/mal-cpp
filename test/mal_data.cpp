#include "doctest.h"
#include <mal.h>
#include <map>
#include <unordered_map>

TEST_SUITE("[mal_data][impl]") {
    TEST_CASE("comparisons") {
        REQUIRE_EQ(mal::READ("123"), mal::READ("123"));
        REQUIRE_EQ(mal::READ("[123]"), mal::READ("[123]"));
        REQUIRE_EQ(mal::READ("(123)"), mal::READ("(123)"));
        REQUIRE_EQ(mal::READ("{jk 543 123 abc}"), mal::READ("{jk 543 123 abc}"));
        REQUIRE_EQ(mal::READ("avc"), mal::READ("avc"));
        REQUIRE_EQ(mal::READ(R"("abc")"), mal::READ(R"("abc")"));
        REQUIRE_EQ(mal::READ(":avc"), mal::READ(":avc"));
    }

    TEST_CASE("unordered map key") {
        auto map = std::unordered_map<mal::MalData, mal::MalData>();
        map[mal::READ("123")] = mal::READ("abc");
        map[mal::READ(R"("hello")")] = mal::READ("(abc)");
        REQUIRE_EQ(map[mal::READ("123")], mal::READ("abc"));
        REQUIRE_EQ(map[mal::READ("839")], mal::READ("nil"));
        REQUIRE_EQ(map[mal::READ(R"("hello")")], mal::READ("(abc)"));
    }

    TEST_CASE("ordered map key") {
        auto map = std::map<mal::MalData, mal::MalData>();
        map[mal::READ("123")] = mal::READ("abc");
        map[mal::READ(R"("hello")")] = mal::READ("(abc)");
        REQUIRE_EQ(map[mal::READ("123")], mal::READ("abc"));
        REQUIRE_EQ(map[mal::READ("839")], mal::READ("nil"));
        REQUIRE_EQ(map[mal::READ(R"("hello")")], mal::READ("(abc)"));
    }
}
