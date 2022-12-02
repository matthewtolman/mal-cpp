#include "mal.h"
#include "variant_overload.hpp"
#include <sstream>
#include <variant>
#include <stack>

static auto print_string_pretty(const mal::MalString& string) -> std::string;
static auto print_list(const mal::MalList& list) -> std::string;
static auto print_vector(const mal::MalVector& vec) -> std::string;
static auto print_map(const mal::MalMap& map) -> std::string;

auto mal::PRINT(const mal::MalData& data, bool print_readably) -> std::string {
    return std::visit(overload{
        [](const mal::MalList& l) { return print_list(l); },
        [](const mal::MalVector& l) { return print_vector(l); },
        [](const mal::MalMap& l) { return print_map(l); },
        [print_readably](const mal::MalString& str) {
            if (print_readably) {
                return print_string_pretty(str);
            }
            else {
                return str.val;
            }
        },
        [](const mal::MalKeyword& k) {
            std::stringstream ss;
            ss << ':' << k.val;
            return ss.str();
        },
        [](const mal::MalNil& n) -> std::string { return "nil"; },
        [](const mal::MalNativeFn& n) -> std::string { return "#<native_function>"; },
        [print_readably](const mal::MalFn& n) -> std::string {
            std::stringstream ss;
            ss << "(fn* [";
            if (!n.bindings.empty()) {
                ss << PRINT(MalData{n.bindings[0]}, print_readably);
                for (auto it = n.bindings.begin() + 1; it != n.bindings.end(); ++it) {
                    ss << " " << PRINT(MalData{*it}, print_readably);
                }
            }
            ss << "] ";
            if (!n.exprs.empty()) {
                ss << PRINT(MalData{n.exprs[0]}, print_readably);
                for (auto it = n.exprs.begin() + 1; it != n.exprs.end(); ++it) {
                    ss << " " << PRINT(MalData{*it}, print_readably);
                }
            }
            ss << ")";
            return ss.str();
        },
        [](const mal::MalBoolean& b) -> std::string { return b.val ? "true" : "false"; },
        [](const auto& v) {
            std::stringstream ss;
            ss << v.val;
            return ss.str();
        }
    }, data.val);
}

auto print_string_pretty(const mal::MalString& string) -> std::string {
    std::stringstream ss;
    ss << '"';
    for (const auto ch : string.val) {
        switch(ch) {
            case '\n': ss << R"(\n)"; break;
            case '\"': ss << R"(\")"; break;
            case '\\': ss << R"(\\)"; break;
            default: ss << ch;
        }
    }
    ss << '"';
    return ss.str();
}

auto print_list(const mal::MalList& list) -> std::string {
    std::stringstream ss;
    ss << "(";
    if (!list.val.empty()) {
        ss << PRINT(list.val[0]);
        std::for_each(list.val.begin() + 1, list.val.end(), [&ss](const auto d) {
            ss << " " << PRINT(d);
        });
    }
    ss << ")";
    return ss.str();
}

auto print_vector(const mal::MalVector& vec) -> std::string {
    std::stringstream ss;
    ss << "[";
    if (!vec.val.empty()) {
        ss << PRINT(vec.val[0]);
        std::for_each(vec.val.begin() + 1, vec.val.end(), [&ss](const auto d) {
            ss << " " << PRINT(d);
        });
    }
    ss << "]";
    return ss.str();
}

auto print_map(const mal::MalMap& map) -> std::string {
    std::stringstream ss;
    ss << "{";
    if (!map.val.empty()) {
        auto it = map.val.begin();
        ss << PRINT(it->first) << " " << PRINT(it->second);
        for(++it; it != map.val.end(); ++it) {
            ss << " " << PRINT(it->first) << " " << PRINT(it->second);
        }
    }
    ss << "}";
    return ss.str();
}
