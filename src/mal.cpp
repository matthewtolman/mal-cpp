#include "mal.h"
#include "variant_overload.hpp"
#include "functions.h"
#include <utility>
#include "hashes.h"
#include <numeric>

std::string mal::rep(std::shared_ptr<MalEnv> env, std::string str, bool print_readably) {
    return PRINT(EVAL(env, READ(std::move(str))), print_readably);
}

auto mal::Reader::next() -> std::optional<std::string> {
    ++position;
    return peek();
}

auto mal::Reader::peek() const -> std::optional<std::string> {
    if (position < tokens.size()) {
        return {tokens[position]};
    }
    return std::nullopt;
}

mal::Reader::Reader(std::vector<std::string> tokens) : tokens(std::move(tokens)) {}

static auto compare_str(const std::string& l, const std::string& r) -> std::strong_ordering {
    auto cmp = l.compare(r);
    if (cmp < 0) {
        return std::strong_ordering::less;
    }
    else if (cmp > 0) {
        return std::strong_ordering::greater;
    }
    else {
        return std::strong_ordering::equivalent;
    }
}

auto operator<=>(const std::optional<mal::MalData>& left, const std::optional<mal::MalData>& right) -> std::strong_ordering {
    if (!left && !right) {
        return std::strong_ordering::equivalent;
    }
    else if (left && right) {
        return (*left) <=> (*right);
    }
    else if (left) {
        return std::strong_ordering::greater;
    }
    else {
        return std::strong_ordering::less;
    }
}

auto mal::MalData::operator<=>(const mal::MalData& right) const -> std::strong_ordering {
    return std::visit(overload{
        [](const mal::MalBoolean& l, const mal::MalBoolean& r) -> std::strong_ordering {
            return l.val <=> r.val;
        },
        [](const mal::MalInteger& l, const mal::MalInteger& r) -> std::strong_ordering {
            return l.val <=> r.val;
        },
        [](const mal::MalKeyword& l, const mal::MalKeyword& r) -> std::strong_ordering {
            return compare_str(l.val, r.val);
        },
        [](const mal::MalSymbol& l, const mal::MalSymbol& r) -> std::strong_ordering {
            return compare_str(l.val, r.val);
        },
        [](const mal::MalString& l, const mal::MalString& r) -> std::strong_ordering {
            return compare_str(l.val, r.val);
        },
        [](const mal::MalList& l, const mal::MalList& r) -> std::strong_ordering {
            auto sizeOrder = l.val.size() <=> r.val.size();
            if (sizeOrder != 0) {
                return sizeOrder;
            }
            for (size_t i = 0; i < l.val.size(); ++i) {
                auto order = l.val[i] <=> r.val[i];
                if (sizeOrder != 0) {
                    return order;
                }
            }
            return std::strong_ordering::equal;
        },
        [](const mal::MalVector& l, const mal::MalVector& r) -> std::strong_ordering {
            auto sizeOrder = l.val.size() <=> r.val.size();
            if (sizeOrder != 0) {
                return sizeOrder;
            }
            for (size_t i = 0; i < l.val.size(); ++i) {
                auto order = l.val[i] <=> r.val[i];
                if (sizeOrder != std::strong_ordering::equivalent && sizeOrder != std::strong_ordering::equal) {
                    return order;
                }
            }
            return std::strong_ordering::equal;
        },
        [](const mal::MalMap& l, const mal::MalMap& r) -> std::strong_ordering {
            auto sizeOrder = l.val.size() <=> r.val.size();
            if (sizeOrder != 0) {
                return sizeOrder;
            }
            for (auto li = l.val.begin(), ri = r.val.begin();
                li != l.val.end() && ri != r.val.end(); ++li, ++ri) {
                auto keyOrder = li->first <=> ri->first;
                if (keyOrder != std::strong_ordering::equivalent && keyOrder != std::strong_ordering::equal) {
                    return keyOrder;
                }
                auto valOrder = li->second <=> ri->second;
                if (valOrder != std::strong_ordering::equivalent && valOrder != std::strong_ordering::equal) {
                    return valOrder;
                }
            }
            return std::strong_ordering::equal;
        },
        [](const mal::MalNativeFn& l, const mal::MalNativeFn& r) -> std::strong_ordering {
            auto sizeOrder = l.num_args <=> r.num_args;
            if (sizeOrder != 0) {
                return sizeOrder;
            }
            auto variadicOrder = l.variadic <=> r.variadic;
            if (variadicOrder != 0) {
                return variadicOrder;
            }
            return fn::get_address(l.fn) <=> fn::get_address(r.fn);
        },
        [](const mal::MalFn& l, const mal::MalFn& r) -> std::strong_ordering {
            auto envOrder = &(*l.closureScope) <=> &(*r.closureScope);
            if (envOrder != 0) {
                return envOrder;
            }
            auto bindingSizeOrder = l.bindings.size() <=> r.bindings.size();
            if (bindingSizeOrder != 0) {
                return bindingSizeOrder;
            }
            for (auto lIt = l.bindings.begin(), rIt = r.bindings.begin(); lIt != l.bindings.end() && rIt != r.bindings.end(); ++lIt, ++rIt) {
                auto order = (*lIt) <=> (*rIt);
                if (order != 0) {
                    return order;
                }
            }
            return MalData{MalVector{l.exprs}} <=> MalVector{r.exprs};
        },
        [&](const auto&, const auto&) -> std::strong_ordering {
            return val.index() <=> right.val.index();
        }
    }, val, right.val);
}

auto mal::hash(const mal::MalData &obj) -> size_t {
    auto index = obj.val.index();
    return std::visit(overload{
            [&index](const mal::MalList &o) {
                std::vector<size_t> hashes{};
                hashes.reserve(o.val.size());
                std::transform(o.val.begin(), o.val.end(), std::back_inserter(hashes), [](const auto &d) {
                    return std::hash<mal::MalData>{}(d);
                });
                return std::accumulate(hashes.begin(), hashes.end(), index,
                                       [](const auto &a, const auto &c) { return hashes::hash_combine(a, c); });
            },
            [&index](const mal::MalVector &o) {
                std::vector<size_t> hashes{};
                hashes.reserve(o.val.size());
                std::transform(o.val.begin(), o.val.end(), std::back_inserter(hashes), [](const auto &d) {
                    return std::hash<mal::MalData>{}(d);
                });
                return std::accumulate(hashes.begin(), hashes.end(), index,
                                       [](const auto &a, const auto &c) { return hashes::hash_combine(a, c); });
            },
            [](const mal::MalNil &o) {
                return std::numeric_limits<size_t>::max();
            },
            [&index](const mal::MalMap &o) {
                auto hash = index;
                for (const auto &it: o.val) {
                    hash = hashes::hash_combine(hash, it.first, it.second);
                }
                return hash;
            },
            [&index](const mal::MalNativeFn &o) {
                return hashes::hash_combine(index, o.num_args, o.variadic);
            },
            [&index](const mal::MalFn& o) {
                return hashes::hash_combine(index, o.closureScope, o.bindings.size(), o.exprs.size());
            },
            [&index](const auto &o) {
                return hashes::hash_combine(index, o.val);
            }
    }, obj.val);
}


auto mal::MalNativeFn::operator()(std::shared_ptr<MalEnv> env, const mal::MalList& args) const -> MalData {
    if (args.val.size() < num_args || (!variadic && args.val.size() > num_args)) {
        throw std::runtime_error("Expected '" + std::to_string(num_args) + "' args to func call, received '" + std::to_string(args.val.size()) + "' instead!");
    }
    return fn(std::move(env), args);
}

auto mal::MalSymbol::operator<=>(const mal::MalSymbol &right) const -> std::strong_ordering {
    return compare_str(val, right.val);
}
