#include "mal.h"
#include "variant_overload.hpp"
#include <sstream>
#include <iostream>
#include <fstream>
#include <set>

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
static auto map(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData;
static auto is_symbol(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData;
static auto is_keyword(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData;
static auto is_hash_map(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData;
static auto hash_map(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData;
static auto symbol(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData;
static auto keyword(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData;
static auto assoc(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData;
static auto dissoc(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData;
static auto get(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData;
static auto contains(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData;
static auto entries(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData;
static auto values(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData;

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
                    {mal::MalSymbol{"deref"}, mal::MalNativeFn{1, false, deref}},
                    {mal::MalSymbol{"reset!"}, mal::MalNativeFn{2, false, reset}},
                    {mal::MalSymbol{"swap!"}, mal::MalNativeFn{2, true, swap}},
                    {mal::MalSymbol{"cons"}, mal::MalNativeFn{2, false, cons}},
                    {mal::MalSymbol{"concat"}, mal::MalNativeFn{0, true, concat}},
                    {mal::MalSymbol{"nth"}, mal::MalNativeFn{2, false, nth}},
                    {mal::MalSymbol{"first"}, mal::MalNativeFn{1, false, first}},
                    {mal::MalSymbol{"rest"}, mal::MalNativeFn{1, false, rest}},
                    {mal::MalSymbol{"throw"}, mal::MalNativeFn{1, false, throw_err}},
                    {mal::MalSymbol{"apply"}, mal::MalNativeFn{1, true, apply}},
                    {mal::MalSymbol{"map"}, mal::MalNativeFn{2, true, map}},
                    {mal::MalSymbol{"vec"}, mal::MalNativeFn{0, true, vector}},
                    {mal::MalSymbol{"list?"}, mal::MalNativeFn{1, false, is_list}},
                    {mal::MalSymbol{"vec?"}, mal::MalNativeFn{1, false, is_vector}},
                    {mal::MalSymbol{"empty?"}, mal::MalNativeFn{1, false, is_empty}},
                    {mal::MalSymbol{"atom?"}, mal::MalNativeFn{1, false, is_atom}},
                    {mal::MalSymbol{"macro?"}, mal::MalNativeFn{1, false, is_macro}},
                    {mal::MalSymbol{"symbol?"}, mal::MalNativeFn{1, false, is_symbol}},
                    {mal::MalSymbol{"keyword?"}, mal::MalNativeFn{1, false, is_keyword}},
                    {mal::MalSymbol{"hash-map?"}, mal::MalNativeFn{1, false, is_hash_map}},
                    {mal::MalSymbol{"hash-map"}, mal::MalNativeFn{0, true, hash_map}},
                    {mal::MalSymbol{"symbol"}, mal::MalNativeFn{1, true, symbol}},
                    {mal::MalSymbol{"keyword"}, mal::MalNativeFn{1, true, keyword}},
                    {mal::MalSymbol{"assoc"}, mal::MalNativeFn{3, true, assoc}},
                    {mal::MalSymbol{"dissoc"}, mal::MalNativeFn{2, true, dissoc}},
                    {mal::MalSymbol{"get"}, mal::MalNativeFn{2, false, get}},
                    {mal::MalSymbol{"entries"}, mal::MalNativeFn{1, false, entries}},
                    {mal::MalSymbol{"values"}, mal::MalNativeFn{1, false, values}},
                    {mal::MalSymbol{"contains?"}, mal::MalNativeFn{2, false, contains}},
            }
    );
    mal::EVAL(env, mal::READ("(def! load-file (fn* (*f*) (eval (read-string (str \"(do \" (slurp *f*) \"\\nnil)\")))))"));
    mal::EVAL(env, mal::READ("(defmacro! cond (fn* (& xs) (if (> (count xs) 0) (list 'if (first xs) (if (> (count xs) 1) (nth xs 1) (throw \"odd number of forms to cond\")) (cons 'cond (rest (rest xs)))))))"));
    mal::EVAL(env, mal::READ("(def! nil? (fn* (v) (= v nil)))"));
    mal::EVAL(env, mal::READ("(def! true? (fn* (v) (= v true)))"));
    mal::EVAL(env, mal::READ("(def! false? (fn* (v) (= v false)))"));
    mal::EVAL(env, mal::READ("(def! truthy? (fn* (v) (if v true false)))"));
    mal::EVAL(env, mal::READ("(def! not (fn* (v) (if v false true)))"));
    mal::EVAL(env, mal::READ("(def! falsey? (fn* (v) (not v)))"));
    mal::EVAL(env, mal::READ("(def! seq? (fn* [v] (or (list? v) (vec? v))))"));
    mal::EVAL(env, mal::READ("(def! keys (fn* [v] (map first v)))"));
    mal::EVAL(env, mal::READ("(def! second (fn* [v] (nth v 1)))"));
    return mal::MalNs{
        "core",
        env->definitions()
    };
}

static auto values(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData {
    if (!args.val[0].is_map()) {
        throw std::runtime_error("Expected argument 1 to entries to be a map");
    }
    auto res = mal::MalList{};
    res.val.reserve(std::get<mal::MalMap>(args.val[0].val).val.size());
    for (const auto& [key, value] : std::get<mal::MalMap>(args.val[0].val).val) {
        res.val.emplace_back(value);
    }
    return res;
}

static auto entries(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData {
    if (!args.val[0].is_map()) {
        throw std::runtime_error("Expected argument 1 to entries to be a map");
    }
    auto res = mal::MalList{};
    res.val.reserve(std::get<mal::MalMap>(args.val[0].val).val.size());
    for (const auto& [key, value] : std::get<mal::MalMap>(args.val[0].val).val) {
        res.val.emplace_back(mal::MalVector{{key, value}});
    }
    return res;
}

static auto contains(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData {
    if (!args.val[0].is_map()) {
        throw std::runtime_error("Expected argument 1 to contains? to be a map");
    }
    return mal::MalBoolean{std::get<mal::MalMap>(args.val[0].val).val.contains(args.val[1])};
}

static auto get(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData {
    if (!args.val[0].is_map()) {
        throw std::runtime_error("Expected argument 1 to get to be a map");
    }
    const auto& mp = std::get<mal::MalMap>(args.val[0].val).val;
    if (mp.contains(args.val[1])) {
        return mp.at(args.val[1]);
    }
    else {
        return mal::MalData{};
    }
}

static auto assoc(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData {
    if (!args.val[0].is_map()) {
        throw std::runtime_error("Expected argument 1 to assoc to be a map");
    }
    mal::MalMap mp = std::get<mal::MalMap>(args.val[0].val);
    for (size_t i = 1; i < args.val.size(); i += 2) {
        auto key = args.val[i];
        auto val = i + 1 < args.val.size() ? args.val[i + 1] : mal::MalData{};
        mp.val[key] = val;
    }
    return mp;
}

static auto dissoc(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData {
    if (!args.val[0].is_map()) {
        throw std::runtime_error("Expected argument 1 to dissoc to be a map");
    }
    mal::MalMap mp{};
    std::set<mal::MalData> ignoreKeys{};
    for (size_t i = 1; i < args.val.size(); ++i) {
        ignoreKeys.insert(args.val[i]);
    }
    for (const auto& [key, val] : std::get<mal::MalMap>(args.val[0].val).val) {
        if (!ignoreKeys.contains(key)) {
            mp.val[key] = val;
        }
    }
    return mp;
}

static auto symbol(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData {
    if (!args.val[0].is_string()) {
        throw std::runtime_error("Expected argument 1 to symbol to be a string");
    }
    return mal::MalSymbol{std::get<mal::MalString>(args.val[0].val).val};
}

static auto keyword(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData {
    if (!args.val[0].is_string()) {
        throw std::runtime_error("Expected argument 1 to keyword to be a string");
    }
    return mal::MalKeyword{std::get<mal::MalString>(args.val[0].val).val};
}

static auto hash_map(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData {
    mal::MalMap res;
    for (size_t i = 0; i < args.val.size(); i += 2) {
        auto key = args.val[i];
        auto val = i + 1 < args.val.size() ? args.val[i + 1] : mal::MalData{};
        res.val[key] = val;
    }
    return res;
}

static auto is_hash_map(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData {
    return mal::MalBoolean{args.val[0].is_map()};
}

static auto is_keyword(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData {
    return mal::MalBoolean{args.val[0].is_keyword()};
}

static auto is_symbol(std::shared_ptr<mal::MalEnv> env, const mal::MalList& args) -> mal::MalData {
    return mal::MalBoolean{args.val[0].is_symbol()};
}

static auto map(std::shared_ptr<mal::MalEnv> env, const mal::MalList &list) -> mal::MalData {
    if (!list.val[0].is_fn()) {
        throw std::runtime_error("Argument 1 to map must be a function!");
    }

    size_t maxSize = 0;
    for (size_t i = 1; i < list.val.size(); ++i) {
        if (!list.val[i].is_seq() && !list.val[i].is_string() && !list.val[i].is_map() && !list.val[i].is_nil()) {
            throw std::runtime_error("Argument " + std::to_string(i + 1) + " to map must be a list, vector, string, or map!");
        }
        auto size = std::visit(overload {
            [](const mal::MalList& l) -> size_t { return l.val.size(); },
            [](const mal::MalVector& l) -> size_t { return l.val.size(); },
            [](const mal::MalString& l) -> size_t { return l.val.size(); },
            [](const mal::MalMap& l) -> size_t { return l.val.size(); },
            [](const auto& l) -> size_t { return 0; },
        }, list.val[i].val);
        maxSize = std::max(maxSize, size);
    }

    std::vector<decltype(mal::MalMap::val)::const_iterator> mapIterators{};
    mapIterators.reserve(list.val.size());
    for (size_t seqIndex = 1; seqIndex < list.val.size(); ++seqIndex) {
        if (list.val[seqIndex].is_map()) {
            mapIterators[seqIndex] = std::get<mal::MalMap>(list.val[seqIndex].val).val.begin();
        }
    }

    std::function<mal::MalData (const mal::MalList&)> fn;
    if (list.val[0].is_user_fn()) {
        fn = [&list, &env](const mal::MalList& args) -> mal::MalData { return std::get<mal::MalFn>(list.val[0].val)(env, args); };
    }
    else {
        fn = [&list, &env](const mal::MalList& args) -> mal::MalData { return std::get<mal::MalNativeFn>(list.val[0].val)(env, args); };
    }

    mal::MalList res;
    res.val.reserve(maxSize);
    for (size_t elemIndex = 0; elemIndex < maxSize; ++elemIndex) {
        mal::MalList args;
        args.val.reserve(list.val.size() - 1);
        for (size_t seqIndex = 1; seqIndex < list.val.size(); ++seqIndex) {
            auto a = std::visit(overload{
                [&elemIndex](const mal::MalList& o) {
                        return elemIndex < o.val.size() ? o.val[elemIndex] : mal::MalData{};
                },
                [&elemIndex](const mal::MalVector& o) {
                    return elemIndex < o.val.size() ? o.val[elemIndex] : mal::MalData{};
                },
                [&elemIndex](const mal::MalString& o) {
                    return elemIndex < o.val.size() ? mal::MalString{std::string(1, o.val[elemIndex])} : mal::MalData{};
                },
                [&mapIterators, &seqIndex](const mal::MalMap& o) {
                    auto& it = mapIterators[seqIndex];
                    if (it == o.val.end()) {
                        return mal::MalData{};
                    }
                    auto entry = *it;
                    auto res = mal::MalVector{{
                        entry.first,
                        entry.second}};
                    ++it;
                    return mal::MalData{res};
                },
                [](const auto& o) { return mal::MalData{}; }
            }, list.val[seqIndex].val);

            args.val.emplace_back(a);
        }
        res.val.emplace_back(fn(args));
    }
    return res;
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
    const auto& vec = (args.val[0].is_list()
                ? std::get<mal::MalList>(args.val[0].val).val
                : std::get<mal::MalVector>(args.val[0].val).val);
    const auto index = std::get<mal::MalInteger>(args.val[1].val).val;
    if (index < vec.size()) {
        return vec[index];
    }
    return mal::MalData{};
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
