#include "mal.h"
#include <utility>

static auto eval_ast(std::shared_ptr<mal::MalEnv> env, mal::MalData ast) -> mal::MalData;
static auto pair_list(const std::vector<mal::MalData>& data) -> std::vector<std::pair<mal::MalSymbol, mal::MalData>>;
static auto map_entries(std::map<mal::MalData, mal::MalData> data) -> std::vector<std::pair<mal::MalSymbol, mal::MalData>>;
static auto quasiquote(std::shared_ptr<mal::MalEnv> env, mal::MalData ast) -> mal::MalData;
static auto macro_expand(std::shared_ptr<mal::MalEnv> env, mal::MalData ast) -> mal::MalData;
static auto is_macro_call(std::shared_ptr<mal::MalEnv> env, const mal::MalData& ast) -> bool;

auto mal::EVAL(std::shared_ptr<MalEnv> env, mal::MalData data) -> mal::MalData {
    while (true) {
        if (data.is_list()) {
            data = macro_expand(env, data);
            const auto &list = std::get<MalList>(data.val);
            if (list.val.empty()) {
                return data;
            }
            else if (list.val[0] == MalSymbol{"def!"}) {
                if (list.val.size() != 3) {
                    throw std::runtime_error(
                            "Expected 2 arguments to 'def!', received " + std::to_string(list.val.size() - 1));
                } else if (!list.val[1].is_symbol()) {
                    throw std::runtime_error("Expected first argument to 'def!' to be a symbol!");
                } else {
                    auto& dEnv = env->root();
                    return dEnv[std::get<MalSymbol>(list.val[1].val)] = EVAL(env, list.val[2]);
                }
            }
            else if (list.val[0] == MalSymbol{"defmacro!"}) {
                if (list.val.size() != 3) {
                    throw std::runtime_error(
                            "Expected 2 arguments to 'def!', received " + std::to_string(list.val.size() - 1));
                } else if (!list.val[1].is_symbol()) {
                    throw std::runtime_error("Expected first argument to 'def!' to be a symbol!");
                } else {
                    auto& dEnv = env->root();
                    auto val = EVAL(env, list.val[2]);
                    if (!val.is_user_fn()) {
                        throw std::runtime_error("Macros must be functions!");
                    }
                    std::get<mal::MalFn>(val.val).is_macro = true;
                    return dEnv[std::get<MalSymbol>(list.val[1].val)] = val;
                }
            }
            else if (list.val[0] == MalSymbol{"quasiquoteexpand"}) {
                if (!list.val[1].is_list() || std::get<mal::MalList>(list.val[1].val).val.size() != 2 || std::get<mal::MalList>(list.val[1].val).val[0] != mal::MalSymbol{"quasiquote"}) {
                    throw std::runtime_error("Expected quasiquoteexpend in form (quasiquoteexpand (quasiquote <to-expand>))");
                }
                return quasiquote(env, std::get<mal::MalList>(list.val[1].val).val[1]);
            }
            else if (list.val[0] == MalSymbol{"quasiquote"}) {
                auto quoted = mal::quasiquote(env, list.val[1]);
                std::swap(data, quoted);
            }
            else if (list.val[0] == MalSymbol{"macroexpand"}) {
                return macro_expand(env, list.val[1]);
            }
            else if (list.val[0] == MalSymbol{"quote"}) {
                return list.val[1];
            }
            else if (list.val[0] == MalSymbol{"try*"}) {
                const auto& c = std::get<mal::MalList>(list.val[2].val);
                if (c.val[0] != MalSymbol{"catch*"} || !c.val[1].is_symbol()) {
                    throw std::runtime_error("Invalid catch* for try*");
                }
                try {
                    return EVAL(env, list.val[1]);
                }
                catch (const std::runtime_error& e) {
                    auto sym = std::get<mal::MalSymbol>(c.val[1].val);
                    env = std::make_shared<mal::MalEnv>(env);
                    (*env)[sym] = MalString{e.what()};
                    auto cpy = c.val[2];
                    std::swap(data, cpy);
                }
            }
            else if (list.val[0] == MalSymbol{"let*"}) {
                if (list.val.size() != 3) {
                    throw std::runtime_error(
                            "Expected 2 arguments to 'def!', received " + std::to_string(list.val.size() - 1));
                } else if (!list.val[1].is_seq() && !list.val[1].is_map()) {
                    throw std::runtime_error("Expected first argument to 'let*' to be a list, vector, or map!");
                } else {
                    auto bindings = list.val[1].is_vec()
                                    ? pair_list(std::get<MalVector>(list.val[1].val).val)
                                    : list.val[1].is_list()
                                      ? pair_list(std::get<MalList>(list.val[1].val).val)
                                      : map_entries(std::get<MalMap>(list.val[1].val).val);
                    env = std::make_shared<MalEnv>(std::move(env));
                    for (const auto &[key, val]: bindings) {
                        (*env)[key] = EVAL(env, val);
                    }
                    auto cpy = list.val[2];
                    std::swap(cpy, data);
                }
            }
            else if (list.val[0] == MalSymbol{"do"}) {
                if (list.val.size() < 2) {
                    throw std::runtime_error(
                            "Expected at least 1 arguments to 'do', received " + std::to_string(list.val.size() - 1));
                }
                auto last = MalData{};
                for (size_t i = 1; i < list.val.size() - 1; ++i) {
                    EVAL(env, list.val[i]);
                }
                auto cpy = list.val[list.val.size() - 1];
                std::swap(data, cpy);
            }
            else if (list.val[0] == MalSymbol{"if"}) {
                if (list.val.size() < 3 || list.val.size() > 4) {
                    throw std::runtime_error(
                            "Expected at 3-4 arguments to 'if', received " + std::to_string(list.val.size() - 1));
                }
                auto cond = EVAL(env, list.val[1]);
                if (cond.is_truthy()) {
                    auto cpy = list.val[2];
                    std::swap(cpy, data);
                } else {
                    auto cpy = list.val.size() > 3 ? list.val[3] : MalData{};
                    std::swap(cpy, data);
                }
            }
            else if (list.val[0] == MalSymbol{"fn*"}) {
                if (list.val.size() < 3) {
                    throw std::runtime_error(
                            "Expected at least 3 arguments to 'fn*', received " + std::to_string(list.val.size() - 1));
                }
                if (!list.val[1].is_seq()) {
                    throw std::runtime_error("Expected argument 2 to 'fn*' to be a sequence of values!");
                }
                auto argsData = list.val[1].is_vec() ? std::get<MalVector>(list.val[1].val).val : std::get<MalList>(
                        list.val[1].val).val;
                auto args = std::vector<MalSymbol>{};
                args.reserve(argsData.size());
                for (const auto &data: argsData) {
                    if (!data.is_symbol()) {
                        throw std::runtime_error(
                                "Expected symbol in bindings arg for 'fn*', received " + PRINT(data, true) + "!");
                    }
                    args.emplace_back(std::get<MalSymbol>(data.val));
                }
                auto exprs = std::vector<MalData>{};
                exprs.reserve(list.val.size() - 2);
                for (size_t i = 2; i < list.val.size(); ++i) {
                    exprs.emplace_back(list.val[i]);
                }
                return MalFn{
                                args,
                                exprs,
                                env
                        };
            }
            else {
                auto evaluated_list = eval_ast(env, data);
                auto eval_list = std::get<MalList>(evaluated_list.val);
                const auto &first = eval_list.val[0];
                if (!first.is_fn()) {
                    throw std::runtime_error("Invalid function " + mal::PRINT(first, true));
                }
                if (first.is_native_fn()) {
                    auto fn = std::get<MalNativeFn>(first.val);
                    eval_list.val.erase(eval_list.val.begin(), eval_list.val.begin() + 1);
                    return fn(env, eval_list);
                } else {
                    auto fn = std::get<MalFn>(first.val);
                    eval_list.val.erase(eval_list.val.begin(), eval_list.val.begin() + 1);

                    if (eval_list.val.size() != fn.bindings.size()) {
                        throw std::runtime_error("Expected '" + std::to_string(fn.bindings.size()) + "' args to func call, received '" + std::to_string(eval_list.val.size()) + "' instead!");
                    }
                    env = std::make_shared<MalEnv>(env);
                    env->mergeWith(fn.closureScope);
                    for (size_t i = 0; i < fn.bindings.size(); ++i) {
                        (*env)[fn.bindings[i]] = EVAL(env, eval_list.val[i]);
                    }
                    for (size_t i = 1; i < fn.exprs.size() - 1; ++i) {
                        EVAL(env, fn.exprs[i]);
                    }
                    auto cpy = fn.exprs[fn.exprs.size() - 1];
                    std::swap(data, cpy);
                }
            }
        } else {
            return eval_ast(env, data);
        }
    }
}

mal::MalEnv::MalEnv() : defined(mal::core_ns().mappings) {}

static auto eval_ast(std::shared_ptr<mal::MalEnv> env, mal::MalData ast) -> mal::MalData {
    using namespace mal;
    if (ast.is_symbol()) {
        return (*env)[std::get<MalSymbol>(ast.val)];
    }
    else if (ast.is_list()) {
        std::vector<MalData> res{};
        const auto& list = std::get<MalList>(ast.val);
        res.reserve(list.val.size());
        std::transform(list.val.begin(), list.val.end(), std::back_inserter(res), [&env](const auto& data) {
            return EVAL(env, data);
        });
        return MalList{res};
    }
    else if (ast.is_vec()) {
        std::vector<MalData> res{};
        const auto& vector = std::get<MalVector>(ast.val);
        res.reserve(vector.val.size());
        std::transform(vector.val.begin(), vector.val.end(), std::back_inserter(res), [&env](const auto& data) {
            return EVAL(env, data);
        });
        return MalVector{res};
    }
    else if (ast.is_map()) {
        std::map<MalData, MalData> res{};
        const auto& map = std::get<MalMap>(ast.val);
        std::for_each(map.val.begin(), map.val.end(), [&env, &res](const auto& entry) {
            res[EVAL(env, entry.first)] = EVAL(env, entry.second);;
        });
        return MalMap{res};
    }
    else {
        return ast;
    }
}

auto pair_list(const std::vector<mal::MalData> &data) -> std::vector<std::pair<mal::MalSymbol, mal::MalData>> {
    if ((data.size() % 2) != 0) {
        throw std::runtime_error("Bindings must have an even number of entries");
    }
    std::vector<std::pair<mal::MalSymbol, mal::MalData>> res;
    res.reserve(data.size() / 2);
    for (size_t i = 0; i < data.size() && i + 1 < data.size(); i += 2) {
        auto key = data[i];
        auto val = data[i + 1];
        if (!key.is_symbol()) {
            throw std::runtime_error("Cannot bind to value " + mal::PRINT(key, true) + ", expected symbol!");
        }
        res.emplace_back(std::get<mal::MalSymbol>(key.val), val);
    }
    return res;
}

auto map_entries(std::map<mal::MalData, mal::MalData> data) -> std::vector<std::pair<mal::MalSymbol, mal::MalData>> {
    std::vector<std::pair<mal::MalSymbol, mal::MalData>> res;
    res.reserve(data.size());
    for (const auto& [key, val] : data) {
        if (!key.is_symbol()) {
            throw std::runtime_error("Cannot bind to value " + mal::PRINT(key, true) + ", expected symbol!");
        }
        res.emplace_back(std::get<mal::MalSymbol>(key.val), val);
    }
    return res;
}

auto mal::quasiquote(std::shared_ptr<mal::MalEnv> env, mal::MalData ast) -> mal::MalData {
    if (ast.is_list()) {
        const auto& list = std::get<mal::MalList>(ast.val);
        if (!list.val.empty() && list.val[0] == mal::MalSymbol{"unquote"}) {
            return list.val[1];
        }
        else {
            auto res = mal::MalList{};
            for (auto eltIt = list.val.rbegin(); eltIt != list.val.rend(); ++eltIt) {
                if (eltIt->is_list()) {
                    const auto& eltList = std::get<mal::MalList>(eltIt->val);
                    if (!eltList.val.empty() && eltList.val[0] == mal::MalSymbol("splice-unquote")) {
                        auto cpy = mal::MalList{{mal::MalSymbol{"concat"}, eltList.val[1], res}};
                        std::swap(cpy, res);
                        continue;
                    }
                }
                auto cpy = mal::MalList{{mal::MalSymbol{"cons"}, mal::MalList{{mal::MalSymbol{"quasiquote"}, *eltIt}}, res}};
                std::swap(cpy, res);
            }
            return res;
        }
    }
    else {
        return mal::MalList{{mal::MalSymbol{"quote"}, ast}};
    }
}

auto macro_expand(std::shared_ptr<mal::MalEnv> env, mal::MalData ast) -> mal::MalData {
    while (is_macro_call(env, ast)) {
        auto list = std::get<mal::MalList>(ast.val);
        mal::MalData mapping = (*env)[std::get<mal::MalSymbol>(list.val[0].val)];
        auto call = std::get<mal::MalFn>(mapping.val);
        list.val.erase(list.val.begin(), list.val.begin() + 1);
        ast = call(env, list);
    }
    return ast;
}

auto is_macro_call(std::shared_ptr<mal::MalEnv> env, const mal::MalData& ast) -> bool {
    if (!ast.is_list()) {
        return false;
    }
    const auto& list = std::get<mal::MalList>(ast.val);
    if (list.val.empty()) {
        return false;
    }
    if (!list.val[0].is_symbol()) {
        return false;
    }
    const auto& sym = std::get<mal::MalSymbol>(list.val[0].val);
    if (!env->contains(sym)) {
        return false;
    }
    mal::MalData mapping = (*env)[sym];
    return mapping.is_macro();
}
