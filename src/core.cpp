#include "mal.h"
#include <sstream>
#include <iostream>

static auto add(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;
static auto sub(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;
static auto mul(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;
static auto divide(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;
static auto prn(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;
static auto pr_str(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;
static auto str(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;
static auto println(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;

auto mal::core_ns() -> mal::MalNs {
    return mal::MalNs{
        "core",
        {
                {mal::MalSymbol{"+"}, mal::MalData{mal::MalNativeFn{0, true, add}}},
                {mal::MalSymbol{"-"}, mal::MalData{mal::MalNativeFn{0, true, sub}}},
                {mal::MalSymbol{"*"}, mal::MalData{mal::MalNativeFn{0, true, mul}}},
                {mal::MalSymbol{"/"}, mal::MalData{mal::MalNativeFn{0, true, divide}}},
                {mal::MalSymbol{"prn"}, mal::MalData{mal::MalNativeFn{0, true, prn}}},
                {mal::MalSymbol{"pr-str"}, mal::MalData{mal::MalNativeFn{0, true, pr_str}}},
                {mal::MalSymbol{"str"}, mal::MalData{mal::MalNativeFn{0, true, str}}},
                {mal::MalSymbol{"println"}, mal::MalData{mal::MalNativeFn{0, true, println}}},
        }
    };
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
    return MalData{MalString{ss.str()}};
}

auto str(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData {
    using namespace mal;
    std::stringstream ss;
    if (!list.val.empty()) {
        for (auto it = list.val.begin(); it != list.val.end(); ++it) {
            ss << PRINT(*it, false);
        }
    }
    return MalData{MalString{ss.str()}};
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
            res = mal::MalData{mal::MalList{rVec}};
        }
        else if (v.is_map() && res.is_map()) {
            auto &rMap = std::get<MalMap>(res.val);
            const auto &map = std::get<MalMap>(v.val).val;
            rMap.val.insert(map.begin(), map.end());
        }
        else if (v.is_map() || res.is_map()) {
            res = MalData{MalList{{res, v}}};
        }
        else if (v.is_string() || res.is_string()) {
            if (v.is_string() && res.is_string()) {
                std::get<MalString>(res.val).val += std::get<MalString>(v.val).val;
            } else if (v.is_string()) {
                res = MalData{MalString{mal::PRINT(res, true) + std::get<MalString>(v.val).val}};
            } else  {
                std::get<MalString>(res.val).val += mal::PRINT(v, true);
            }
        } else if (v.is_int() && res.is_int()) {
            std::get<MalInteger>(res.val).val += std::get<MalInteger>(v.val).val;
        } else {
            res = MalData{
                    MalString{
                            PRINT(res, true) + PRINT(v, true)
                    }
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
