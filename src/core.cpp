#include "mal.h"
#include <sstream>
#include <iostream>
#include <fstream>

static auto add(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;
static auto sub(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;
static auto mul(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;
static auto divide(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;
static auto prn(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;
static auto pr_str(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;
static auto str(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;
static auto println(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;
static auto read_str(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;
static auto slurp(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;
static auto list(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;
static auto is_list(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;
static auto is_empty(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;
static auto count(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;
static auto equals(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;
static auto less(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;
static auto less_eq(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;
static auto greater(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;
static auto greater_eq(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;
static auto compare(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;

auto mal::core_ns() -> mal::MalNs {
    return mal::MalNs{
        "core",
        {
                {mal::MalSymbol{"+"}, mal::MalNativeFn{0, true, add}},
                {mal::MalSymbol{"-"}, mal::MalNativeFn{0, true, sub}},
                {mal::MalSymbol{"*"}, mal::MalNativeFn{0, true, mul}},
                {mal::MalSymbol{"/"}, mal::MalNativeFn{0, true, divide}},
                {mal::MalSymbol{"prn"}, mal::MalNativeFn{0, true, prn}},
                {mal::MalSymbol{"pr-str"}, mal::MalNativeFn{0, true, pr_str}},
                {mal::MalSymbol{"str"}, mal::MalNativeFn{0, true, str}},
                {mal::MalSymbol{"println"}, mal::MalNativeFn{0, true, println}},
                {mal::MalSymbol{"read-str"},mal::MalNativeFn{1, false, read_str}},
                {mal::MalSymbol{"slurp"}, mal::MalNativeFn{1, false, slurp}},
                {mal::MalSymbol{"list"}, mal::MalNativeFn{0, true, list}},
                {mal::MalSymbol{"list?"}, mal::MalNativeFn{1, false, is_list}},
                {mal::MalSymbol{"empty?"}, mal::MalNativeFn{1, false, is_empty}},
                {mal::MalSymbol{"count"}, mal::MalNativeFn{1, false, count}},
                {mal::MalSymbol{"="}, mal::MalNativeFn{1, true, equals}},
                {mal::MalSymbol{"<"}, mal::MalNativeFn{1, true, less}},
                {mal::MalSymbol{"<="}, mal::MalNativeFn{1, true, less_eq}},
                {mal::MalSymbol{">"}, mal::MalNativeFn{1, true, greater}},
                {mal::MalSymbol{">="}, mal::MalNativeFn{1, true, greater_eq}},
                {mal::MalSymbol{"<=>"}, mal::MalNativeFn{2, false, compare}},
        }
    };
}

auto equals(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData {
    auto cmp = list.val[0];
    for (auto it = list.val.begin() + 1; it != list.val.end(); ++it) {
        if (cmp != *it) {
            return mal::MalBoolean{false};
        }
    }
    return mal::MalBoolean{true};
}

auto compare(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData {
    auto cmp = list.val[0] <=> list.val[1];
    if (cmp == 0) {
        return mal::MalInteger{0};
    }
    else if (cmp < 0) {
        return mal::MalInteger{-1};
    }
    else {
        return mal::MalInteger{1};
    }
}

auto less(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData {
    auto cmp = list.val[0];
    for (auto it = list.val.begin() + 1; it != list.val.end(); ++it) {
        if (cmp >= *it) {
            return mal::MalBoolean{false};
        }
        cmp = *it;
    }
    return mal::MalBoolean{true};
}

auto less_eq(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData {
    auto cmp = list.val[0];
    for (auto it = list.val.begin() + 1; it != list.val.end(); ++it) {
        if (cmp > *it) {
            return mal::MalBoolean{false};
        }
        cmp = *it;
    }
    return mal::MalBoolean{true};
}

auto greater(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData {
    auto cmp = list.val[0];
    for (auto it = list.val.begin() + 1; it != list.val.end(); ++it) {
        if (cmp <= *it) {
            return mal::MalBoolean{false};
        }
        cmp = *it;
    }
    return mal::MalBoolean{true};
}

auto greater_eq(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData {
    auto cmp = list.val[0];
    for (auto it = list.val.begin() + 1; it != list.val.end(); ++it) {
        if (cmp < *it) {
            return mal::MalBoolean{false};
        }
        cmp = *it;
    }
    return mal::MalBoolean{true};
}

auto list(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData {
    return list;
}

auto count(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData {
    if (list.val[0].is_string()) {
        return mal::MalInteger{static_cast<int64_t>(std::get<mal::MalString>(list.val[0].val).val.size())};
    }
    else if (list.val[0].is_list()) {
        return mal::MalInteger{static_cast<int64_t>(std::get<mal::MalList>(list.val[0].val).val.size())};
    }
    else if (list.val[0].is_vec()) {
        return mal::MalInteger{static_cast<int64_t>(std::get<mal::MalVector>(list.val[0].val).val.size())};
    }
    else if (list.val[0].is_map()) {
        return mal::MalInteger{static_cast<int64_t>(std::get<mal::MalMap>(list.val[0].val).val.size())};
    }
    else {
        throw std::runtime_error("Expected a list, map, vector, or string!");
    }
}

auto is_list(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData {
    return mal::MalBoolean{list.val[0].is_list()};
}

auto is_empty(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData {
    if (list.val[0].is_string()) {
        return mal::MalBoolean{std::get<mal::MalString>(list.val[0].val).val.empty()};
    }
    else if (list.val[0].is_list()) {
        return mal::MalBoolean{std::get<mal::MalList>(list.val[0].val).val.empty()};
    }
    else if (list.val[0].is_vec()) {
        return mal::MalBoolean{std::get<mal::MalVector>(list.val[0].val).val.empty()};
    }
    else if (list.val[0].is_map()) {
        return mal::MalBoolean{std::get<mal::MalMap>(list.val[0].val).val.empty()};
    }
    else {
        throw std::runtime_error("Expected a list, map, vector, or string!");
    }
}

auto pr_str(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData {
    using namespace mal;
    std::stringstream ss;
    if (!list.val.empty()) {
        ss << PRINT(list.val[0], true);
        for (auto it = list.val.begin() + 1; it != list.val.end(); ++it) {
            ss << " " << PRINT(*it, true);
        }
    }
    return MalString{ss.str()};
}

auto str(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData {
    using namespace mal;
    std::stringstream ss;
    if (!list.val.empty()) {
        for (auto it = list.val.begin(); it != list.val.end(); ++it) {
            ss << PRINT(*it, false);
        }
    }
    return MalString{ss.str()};
}

auto prn(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData {
    using namespace mal;
    std::cout << std::get<MalString>(pr_str(env, list).val).val << "\n";
    return {};
}

auto println(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData {
    using namespace mal;
    if (!list.val.empty()) {
        std::cout << PRINT(list.val[0], false);
        for (auto it = list.val.begin() + 1; it != list.val.end(); ++it) {
            std::cout << " " << PRINT(*it, false);
        }
    }
    std::cout << "\n";
    return {};
}

auto read_str(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData {
    using namespace mal;
    if (list.val.size() != 1) {
        throw std::runtime_error("Expected 1 argument to read-str, received " + std::to_string(list.val.size()));
    }
    else if (!list.val[0].is_string()) {
        throw std::runtime_error("Expected first argument read-str to be a string, received " + mal::PRINT(list.val[0], true));
    }
    return READ(std::get<MalString>(list.val[0].val).val);
}

auto slurp(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData {
    using namespace mal;
    if (list.val.size() != 1) {
        throw std::runtime_error("Expected 1 argument to slurp, received " + std::to_string(list.val.size()));
    }
    else if (!list.val[0].is_string()) {
        throw std::runtime_error("Expected first argument slurp to be a string, received " + mal::PRINT(list.val[0], true));
    }
    std::ifstream t(std::get<MalString>(list.val[0].val).val);
    std::stringstream buffer;
    buffer << t.rdbuf();
    return MalString{buffer.str()};
}

auto add(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData {
    using namespace mal;

    mal::MalData res;
    for (const auto &v: list.val) {
        if (res.is_nil()) {
            res = v;
        }
        else if (v.is_nil()) {
            continue;
        }
        else if (v.is_seq() || res.is_seq()) {
            const auto &vec = v.is_vec()
                              ? std::get<MalVector>(v.val).val
                              : v.is_list()
                                ? std::get<MalList>(v.val).val
                                : std::vector{v};

            auto rVec = res.is_vec()
                        ? std::get<MalVector>(res.val).val
                        : res.is_list()
                          ? std::get<MalList>(res.val).val
                          : std::vector{res};

            rVec.reserve(rVec.size() + vec.size());
            std::copy(vec.begin(), vec.end(), std::back_inserter(rVec));
            res = mal::MalList{rVec};
        }
        else if (v.is_map() && res.is_map()) {
            auto &rMap = std::get<MalMap>(res.val);
            const auto &map = std::get<MalMap>(v.val).val;
            rMap.val.insert(map.begin(), map.end());
        }
        else if (v.is_map() || res.is_map()) {
            res = MalList{{res, v}};
        }
        else if (v.is_string() || res.is_string()) {
            if (v.is_string() && res.is_string()) {
                std::get<MalString>(res.val).val += std::get<MalString>(v.val).val;
            } else if (v.is_string()) {
                res = MalString{mal::PRINT(res, true) + std::get<MalString>(v.val).val};
            } else  {
                std::get<MalString>(res.val).val += mal::PRINT(v, true);
            }
        } else if (v.is_int() && res.is_int()) {
            std::get<MalInteger>(res.val).val += std::get<MalInteger>(v.val).val;
        } else {
            res = MalString{
                            PRINT(res, true) + PRINT(v, true)
                    };
        }
    }
    return res;
}

auto sub(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData {
    using namespace mal;
    mal::MalData res;
    for (const auto &v: list.val) {
        // Number support only right now
        if (res.is_nil() && v.is_number()) {
            res = v;
        } else if (v.is_nil()) {
            continue;
        } else if (v.is_number()) {
            // TODO: Add numeric casting
            std::get<MalInteger>(res.val).val -= std::get<MalInteger>(v.val).val;
        } else {
            throw std::runtime_error("Unsupported subtraction operation");
        }
    }
    if (list.val.size() == 1) {
        std::get<MalInteger>(res.val).val *= -1;
    }
    return res;
}

auto mul(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData {
    using namespace mal;
    mal::MalData res;
    for (const auto &v: list.val) {
        // Number support only right now
        if (res.is_nil() && v.is_number()) {
            res = v;
        } else if (v.is_nil()) {
            continue;
        } else if (v.is_number()) {
            // TODO: Add numeric casting
            std::get<MalInteger>(res.val).val *= std::get<MalInteger>(v.val).val;
        } else {
            throw std::runtime_error("Unsupported subtraction operation");
        }
    }
    return res;
}

auto divide(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData {
    using namespace mal;
    mal::MalData res;
    for (const auto &v: list.val) {
        // Number support only right now
        if (res.is_nil() && v.is_number()) {
            res = v;
        } else if (v.is_nil()) {
            continue;
        } else if (v.is_number()) {
            // TODO: Add numeric casting
            std::get<MalInteger>(res.val).val /= std::get<MalInteger>(v.val).val;
        } else {
            throw std::runtime_error("Unsupported subtraction operation");
        }
    }
    return res;
}
