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
static auto eval(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;
static auto print_env(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;
static auto get_env(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;
static auto make_atom(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;
static auto is_atom(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;
static auto deref(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;
static auto reset(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;
static auto swap(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;
static auto cons(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;
static auto concat(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData;
static auto is_macro(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData;
static auto is_vector(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData;
static auto vector(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData;
static auto nth(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData;
static auto first(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData;
static auto rest(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData;
static auto throw_err(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData;
static auto apply(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData;

auto mal::core_ns() -> mal::MalNs {
    auto ns = mal::MalNs{
        "core", {}
    };
    auto env = std::make_shared<mal::MalEnv>(
            nullptr,
            std::map<MalSymbol, MalData>{
                    {mal::MalSymbol{"+"}, mal::MalNativeFn{0, true, add}},
                    {mal::MalSymbol{"-"}, mal::MalNativeFn{0, true, sub}},
                    {mal::MalSymbol{"*"}, mal::MalNativeFn{0, true, mul}},
                    {mal::MalSymbol{"/"}, mal::MalNativeFn{0, true, divide}},
                    {mal::MalSymbol{"prn"}, mal::MalNativeFn{0, true, prn}},
                    {mal::MalSymbol{"pr-str"}, mal::MalNativeFn{0, true, pr_str}},
                    {mal::MalSymbol{"str"}, mal::MalNativeFn{0, true, str}},
                    {mal::MalSymbol{"println"}, mal::MalNativeFn{0, true, println}},
                    {mal::MalSymbol{"read-string"},mal::MalNativeFn{1, false, read_str}},
                    {mal::MalSymbol{"slurp"}, mal::MalNativeFn{1, false, slurp}},
                    {mal::MalSymbol{"list"}, mal::MalNativeFn{0, true, list}},
                    {mal::MalSymbol{"list?"}, mal::MalNativeFn{1, false, is_list}},
                    {mal::MalSymbol{"vec"}, mal::MalNativeFn{0, true, vector}},
                    {mal::MalSymbol{"vec?"}, mal::MalNativeFn{1, false, is_vector}},
                    {mal::MalSymbol{"empty?"}, mal::MalNativeFn{1, false, is_empty}},
                    {mal::MalSymbol{"count"}, mal::MalNativeFn{1, false, count}},
                    {mal::MalSymbol{"="}, mal::MalNativeFn{1, true, equals}},
                    {mal::MalSymbol{"<"}, mal::MalNativeFn{1, true, less}},
                    {mal::MalSymbol{"<="}, mal::MalNativeFn{1, true, less_eq}},
                    {mal::MalSymbol{">"}, mal::MalNativeFn{1, true, greater}},
                    {mal::MalSymbol{">="}, mal::MalNativeFn{1, true, greater_eq}},
                    {mal::MalSymbol{"<=>"}, mal::MalNativeFn{2, false, compare}},
                    {mal::MalSymbol{"eval"}, mal::MalNativeFn{1, false,eval}},
                    {mal::MalSymbol{"*print-env*"}, mal::MalNativeFn{0, false, print_env}},
                    {mal::MalSymbol{"*env*"}, mal::MalNativeFn{0, false, get_env}},
                    {mal::MalSymbol{"atom"}, mal::MalNativeFn{1, false, make_atom}},
                    {mal::MalSymbol{"atom?"}, mal::MalNativeFn{1, false, is_atom}},
                    {mal::MalSymbol{"deref"}, mal::MalNativeFn{1, false, deref}},
                    {mal::MalSymbol{"reset!"}, mal::MalNativeFn{2, false, reset}},
                    {mal::MalSymbol{"swap!"}, mal::MalNativeFn{2, true, swap}},
                    {mal::MalSymbol{"cons"}, mal::MalNativeFn{2, false, cons}},
                    {mal::MalSymbol{"concat"}, mal::MalNativeFn{0, true, concat}},
                    {mal::MalSymbol{"macro?"}, mal::MalNativeFn{1, false, is_macro}},
                    {mal::MalSymbol{"nth"}, mal::MalNativeFn{2, false, nth}},
                    {mal::MalSymbol{"first"}, mal::MalNativeFn{1, false, first}},
                    {mal::MalSymbol{"rest"}, mal::MalNativeFn{1, false, rest}},
                    {mal::MalSymbol{"throw"}, mal::MalNativeFn{1, false, throw_err}},
                    {mal::MalSymbol{"apply"}, mal::MalNativeFn{1, true, apply}},
            }
    );
    mal::EVAL(env, mal::READ("(def! load-file (fn* (*f*) (eval (read-string (str \"(do \" (slurp *f*) \"\\nnil)\")))))"));
    mal::EVAL(env, mal::READ("(defmacro! cond (fn* (& xs) (if (> (count xs) 0) (list 'if (first xs) (if (> (count xs) 1) (nth xs 1) (throw \"odd number of forms to cond\")) (cons 'cond (rest (rest xs)))))))"));
    return mal::MalNs{
        "core",
        env->definitions()
    };
}

auto throw_err(std::shared_ptr<mal::MalEnv> env, const mal::MalList& list) -> mal::MalData {
    throw std::runtime_error(mal::PRINT(list.val[0], false));
}

static auto cons(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData {
    if (!list.val[1].is_seq()) {
        throw std::runtime_error("Argument 2 to cons must be a list, vector, or string!");
    }
    auto res = mal::MalList{{list.val[0]}};
    const auto& l = list.val[1].is_list() ? std::get<mal::MalList>(list.val[1].val).val : std::get<mal::MalVector>(list.val[1].val).val;
    res.val.insert(res.val.end(), l.begin(), l.end());
    return res;
}

static auto reset(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData {
    if (!list.val[0].is_atom()) {
        throw std::runtime_error("Argument 1 to reset! must be an atom!");
    }
    auto cpy = std::get<mal::MalAtom>(list.val[0].val);
    cpy.set(list.val[1]);
    return list.val[1];
}

static auto concat(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData {
    if (std::find_if_not(list.val.begin(), list.val.end(), [](const auto& v) { return v.is_seq(); }) != list.val.end()) {
        throw std::runtime_error("All arguments to concat must be lists or vectors");
    }
    auto res = mal::MalList{};
    for(const auto& seq : list.val) {
        const auto& elems = (seq.is_list() ? std::get<mal::MalList>(seq.val).val : std::get<mal::MalVector>(seq.val).val);
        res.val.insert(res.val.end(), elems.begin(), elems.end());
    }
    return res;
}

static auto swap(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData {
    if (!list.val[0].is_atom()) {
        throw std::runtime_error("Argument 1 to swap! must be an atom!");
    }
    else if (!list.val[1].is_fn()) {
        throw std::runtime_error("Argument 2 to swap! must be a function!");
    }
    auto cpy = std::get<mal::MalAtom>(list.val[0].val);
    auto& fnArg = list.val[1];
    auto argList = mal::MalList{};
    argList.val.reserve(list.val.size() - 1);
    argList.val.emplace_back();
    std::copy(list.val.begin() + 2, list.val.end(), std::back_inserter(argList.val));
    return cpy.swap([&env, &fnArg, &argList](const mal::MalData& value) {
        argList.val[0] = value;
        if (fnArg.is_native_fn()) {
            return std::get<mal::MalNativeFn>(fnArg.val)(env, argList);
        }
        else {
            return std::get<mal::MalFn>(fnArg.val)(env, argList);
        }
    });
}

auto deref(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData {
    if (!list.val[0].is_atom()) {
        return list.val[0];
    }
    else {
        return std::get<mal::MalAtom>(list.val[0].val).get();
    }
}

auto is_atom(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData {
    return mal::MalBoolean{list.val[0].is_atom()};
}

auto make_atom(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData {
    return mal::MalAtom(list.val[0]);
}

auto get_env(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData {
    return env->defs();
}

auto print_env(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData {
    return println(env, mal::MalList{{env->defs()}});
}

auto eval(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData {
    return mal::EVAL(std::move(env), list.val[0]);
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
        throw std::runtime_error("Expected 1 argument to read-string, received " + std::to_string(list.val.size()));
    }
    else if (!list.val[0].is_string()) {
        throw std::runtime_error("Expected first argument read-string to be a string, received " + mal::PRINT(list.val[0], true));
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

auto mal::MalAtom::swap(const std::function<MalData(const MalData&)>& fn) -> MalData {
    return ref->swap(fn);
}

auto mal::Atom::swap(const std::function<MalData(const MalData& d)>& fn) -> MalData {
#if THREADING
    std::unique_lock lock(mux);
#endif
    *md = fn(*md);
    return *md;
}

auto is_macro(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData {
    return mal::MalBoolean{args.val[0].is_macro()};
}

auto is_vector(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData {
    return mal::MalBoolean{args.val[0].is_vec()};
}

auto nth(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData {
    if (!args.val[0].is_seq()) {
        throw std::runtime_error("First argument to nth must be a list or vector.");
    }
    if (!args.val[1].is_int()) {
        throw std::runtime_error("Second argument to nth must be an integer.");
    }
    return (args.val[0].is_list()
            ? std::get<mal::MalList>(args.val[0].val).val
            : std::get<mal::MalVector>(args.val[0].val).val)[std::get<mal::MalInteger>(args.val[1].val).val];
}

auto first(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData {
    if (!args.val[0].is_seq()) {
        return mal::MalData{};
    }
    const auto& seq = (args.val[0].is_list()
                       ? std::get<mal::MalList>(args.val[0].val).val
                       : std::get<mal::MalVector>(args.val[0].val).val);
    if (seq.empty()) {
        return mal::MalData{};
    }
    return seq[0];
}

auto rest(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData {
    if (!args.val[0].is_seq()) {
        return mal::MalData{};
    }
    auto seq = (args.val[0].is_list()
                       ? std::get<mal::MalList>(args.val[0].val).val
                       : std::get<mal::MalVector>(args.val[0].val).val);
    if (seq.empty()) {
        return mal::MalData{};
    }
    seq.erase(seq.begin(), seq.begin() + 1);
    return mal::MalList{seq};
}

static auto vector(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData {
    return mal::MalVector{args.val};
}

static auto apply(std::shared_ptr<mal::MalEnv> env, const mal::MalList& list) -> mal::MalData {
    if (!list.val[0].is_fn()) {
        throw std::runtime_error("Expected first argument to apply to be a function!");
    }
    auto args = mal::MalList{};
    args.val.reserve(list.val.size() - 1);
    for(auto it = list.val.begin() + 1; it < list.val.end(); ++it) {
        const auto& e = *it;
        if (e.is_list()) {
            for (const auto& e2 : std::get<mal::MalList>(e.val).val) {
                args.val.emplace_back(e2);
            }
        }
        else if (e.is_vec()) {
            for (const auto &e2: std::get<mal::MalVector>(e.val).val) {
                args.val.emplace_back(e2);
            }
        }
        else {
            args.val.emplace_back(e);
        }
    }
    if (list.val[0].is_user_fn()) {
        return std::get<mal::MalFn>(list.val[0].val)(env, args);
    }
    else {
        return std::get<mal::MalNativeFn>(list.val[0].val)(env, args);
    }
}
