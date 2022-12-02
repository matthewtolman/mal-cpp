#include "mal.h"

#ifndef PCRE2_STATIC
#define PCRE2_STATIC
#endif

#include "jpcre2.hpp"
#include "defer.hpp"
#include "variant_overload.hpp"
#include <stack>
#include <numeric>
#include "hashes.h"

typedef jpcre2::select<char> jp;
static jp::Regex regex(R"([\s,]*(~@|[\[\]{}()'`~^@]|"(?:\\.|[^\\"])*"?|;.*|[^\s\[\]{}('"`,;)]*))", "g");

static auto tokenize(const std::string &str) -> std::vector<std::string> {
    jp::VecNum vec_num;
    jp::RegexMatch rm;
    rm.setRegexObject(&regex)
            .setSubject(str)
            .setNumberedSubstringVector(&vec_num)
            .addModifier("g")
            .match();

    std::vector<std::string> strs{};
    strs.reserve(vec_num.size());
    for (const auto &vec: vec_num) {
        for (auto startIter = vec.begin() + 1; startIter != vec.end(); ++startIter) {
            if (startIter->empty()) {
                continue;
            }
            strs.emplace_back(*startIter);
        }
    }
    return strs;
}

static auto read_list(mal::Reader &reader) -> mal::MalList;

static auto read_vector(mal::Reader &reader) -> mal::MalVector;

static auto read_atom(mal::Reader &reader) -> mal::MalData;

static auto read_string(mal::Reader &reader) -> mal::MalString;

static auto read_form(mal::Reader &reader) -> std::optional<mal::MalData>;

static auto read_keyword(mal::Reader &reader) -> mal::MalKeyword;

static auto read_map(mal::Reader &reader) -> mal::MalMap;

static auto read_str(const std::string &str) -> std::optional<mal::MalData> {
    auto reader = mal::Reader{tokenize(str)};
    return read_form(reader);
}

static auto read_form(mal::Reader &reader) -> std::optional<mal::MalData> {
    auto valOpt = reader.peek();
    if (!valOpt) {
        return std::nullopt;
    }
    auto val = *valOpt;
    if (val == "(") {
        return mal::MalData{read_list(reader)};
    } else if (val == "{") {
        return mal::MalData{read_map(reader)};
    } else if (val == "[") {
        return mal::MalData{read_vector(reader)};
    } else if (val.starts_with('"')) {
        return mal::MalData{read_string(reader)};
    } else if (val.starts_with(':')) {
        return mal::MalData{read_keyword(reader)};
    } else if (val.starts_with(';')) {
        return std::nullopt;
    } else {
        return read_atom(reader);
    }
}

static auto read_list(mal::Reader &reader) -> mal::MalList {
    mal::MalList list;
    while (reader.next() && reader.peek() != ")") {
        auto v = read_form(reader);
        if (v.has_value()) {
            list.val.emplace_back(*v);
        }
    }
    if (reader.peek() != ")") {
        throw std::runtime_error("Expected ')', found end of file!");
    }
    return list;
}

static auto read_vector(mal::Reader &reader) -> mal::MalVector {
    mal::MalVector vec;
    while (reader.next() && reader.peek() != "]") {
        auto v = read_form(reader);
        if (v.has_value()) {
            vec.val.emplace_back(*v);
        }
    }
    if (reader.peek() != "]") {
        throw std::runtime_error("Expected ']', found end of file!");
    }
    return vec;
}

static auto read_map(mal::Reader &reader) -> mal::MalMap {
    mal::MalMap map;
    while (reader.next() && reader.peek() != "}") {
        mal::MalData key;
        {
            bool setKey = false;
            do {
                auto opt = read_form(reader);
                if (opt.has_value()) {
                    key = *opt;
                    setKey = true;
                    break;
                }
            } while (reader.next() && reader.peek() != "}");
            if (!setKey) {
                break;
            }
        }

        if (reader.next() == "}") {
            throw std::runtime_error("Missing value in map! Unexpected '}'");
        } else if (!reader.peek()) {
            throw std::runtime_error("Missing value in map! Unexpected end of file!");
        }

        bool setVal = false;
        do {
            auto opt = read_form(reader);
            if (opt.has_value()) {
                map.val[key] = *opt;
                setVal = true;
                break;
            }
        } while (reader.next() && reader.peek() != "}");
        if (!setVal) {
            if (reader.peek() == "}") {
                throw std::runtime_error("Missing value in map! Unexpected '}'");
            } else if (!reader.peek()) {
                throw std::runtime_error("Missing value in map! Unexpected end of file!");
            }
        }
    }
    if (reader.peek() != "}") {
        throw std::runtime_error("Expected '}', found end of file!");
    }
    return map;
}

static auto read_string(mal::Reader &reader) -> mal::MalString {
    auto val = (*reader.peek());
    val = val.substr(1, val.size() - 2);
    std::stack<std::tuple<size_t, char>> changes{};

    for (size_t i = 0; i < val.size(); ++i) {
        auto ch = val[i];
        if (ch == '\\') {
            auto nextChar = val[i + 1];
            switch (nextChar) {
                case '\\':
                    changes.emplace(std::make_tuple(i, '\\'));
                    break;
                case '"':
                    changes.emplace(std::make_tuple(i, '"'));
                    break;
                case 'n':
                    changes.emplace(std::make_tuple(i, '\n'));
                    break;
                default:
                    changes.emplace(std::make_tuple(i, nextChar));
                    break;
            }
            ++i;
        }
    }

    while (!changes.empty()) {
        auto [index, replacement] = changes.top();
        changes.pop();

        val.erase(val.begin() + index, val.begin() + index + 2);
        val.insert(val.begin() + index, replacement);
    }

    return {val};
}

static auto read_keyword(mal::Reader &reader) -> mal::MalKeyword {
    auto val = *reader.peek();
    return {val.substr(1)};
}

static auto read_atom(mal::Reader &reader) -> mal::MalData {
    auto val = *reader.peek();
    if (val == "nil") {
        return mal::MalData{mal::MalNil{}};
    }
    else if (std::isdigit(val[0]) || (val.size() > 1 && val[0] == '-' && isdigit(val[1]))) {
        return mal::MalData{mal::MalInteger{std::strtoll(val.c_str(), nullptr, 10)}};
    }
    else if (val == "true") {
        return mal::MalData{mal::MalBoolean{true}};
    }
    else if (val == "false") {
        return mal::MalData{mal::MalBoolean{false}};
    }
    else {
        return mal::MalData{mal::MalSymbol{val}};
    }
}

auto mal::READ(std::string str) -> mal::MalData {
    auto res = read_str(str);
    if (!res) {
        return mal::MalData{mal::MalNil{}};
    }
    return *res;
}