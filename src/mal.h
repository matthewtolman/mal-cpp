#pragma once

#include <string>
#include <vector>
#include <optional>
#include <variant>
#include <map>

#if THREADING
#include <thread>
#include <shared_mutex>
#include <memory>
#endif

namespace mal {
    class Reader {
        size_t position = 0;
        std::vector<std::string> tokens;
    public:
        explicit Reader(std::vector<std::string> tokens);
        auto next() -> std::optional<std::string>;
        [[nodiscard]] auto peek() const -> std::optional<std::string>;
    };

    struct MalList;
    struct MalSymbol;
    struct MalInteger;
    struct MalString;
    struct MalKeyword;
    struct MalVector;
    struct MalData;
    struct MalNil;
    struct MalMap;
    struct MalNativeFn;
    struct MalEnv;
    struct MalBoolean;
    struct MalFn;
    struct MalNs;
    struct MalAtom;
    typedef std::variant<MalNil, MalBoolean, MalInteger, MalKeyword, MalSymbol, MalString, MalList, MalVector, MalMap, MalAtom, MalFn, MalNativeFn> MalVariant;

    struct MalNil {};
    struct MalList { std::vector<MalData> val; };
    struct MalSymbol {
        std::string val{};

        MalSymbol() = default;
        MalSymbol(std::string val) : val(std::move(val)) {}
        MalSymbol(const MalSymbol& o) : val(o.val) {}
        MalSymbol(MalSymbol&& o) noexcept : val(std::move(o.val)) {}
        auto operator=(const MalSymbol& o) -> MalSymbol& = default;
        auto operator=(MalSymbol&& o) noexcept -> MalSymbol& {
            std::swap(val, o.val);
            return *this;
        }

        auto operator<=>(const mal::MalSymbol& right) const -> std::strong_ordering;
        inline auto operator==(const mal::MalSymbol& right)const  { return (*this <=> right) == std::strong_ordering::equivalent; }
        inline auto operator!=(const mal::MalSymbol& right) const { return (*this <=> right) != std::strong_ordering::equivalent; }
        inline auto operator<(const mal::MalSymbol& right) const { return (*this <=> right) == std::strong_ordering::less; }
        inline auto operator>(const mal::MalSymbol& right) const { return (*this <=> right) == std::strong_ordering::greater; }
        inline auto operator<=(const mal::MalSymbol& right) const {
            auto cmp = (*this <=> right);
            return cmp == std::strong_ordering::less || cmp == std::strong_ordering::equivalent;
        }
        inline auto operator>=(const mal::MalSymbol& right) const {
            auto cmp = (*this <=> right);
            return cmp == std::strong_ordering::greater || cmp == std::strong_ordering::equivalent;
        }
    };
    struct MalBoolean { bool val; };
    struct MalInteger { int64_t val; };
    struct MalString { std::string val; };
    struct MalKeyword { std::string val; };
    struct MalVector { std::vector<MalData> val; };
    struct MalMap { std::map<MalData, MalData> val; };

    struct Atom{
#if THREADING
        std::shared_mutex mux{};
#endif
        std::unique_ptr<MalData> md = std::make_unique<MalData>();

        auto set(MalData data) -> Atom&;
        auto get() -> MalData;
        auto swap(const std::function<MalData(const MalData&)>&) -> MalData;
    };

    class MalAtom {
        std::shared_ptr<Atom> ref;
    public:
        MalAtom(MalData md);

        auto get() const -> MalData;
        auto set(MalData md) -> MalAtom&;
        auto swap(const std::function<MalData(const MalData&)>&) -> MalData;
    };
    struct MalFn {
        std::vector<MalSymbol> bindings;
        std::vector<MalData> exprs;
        std::shared_ptr<MalEnv> closureScope;
        bool is_macro = false;
        auto operator()(std::shared_ptr<MalEnv> env, const mal::MalList& args) const -> MalData;
    };
    struct MalNativeFn {
        size_t num_args;
        bool variadic;
        std::function<MalData (std::shared_ptr<MalEnv>, const MalList&)> fn;
        auto operator()(std::shared_ptr<MalEnv> env, const mal::MalList& args) const -> MalData;
    };
    struct MalData {
        MalVariant val = MalNil{};

        MalData() = default;
        MalData(MalNil v) : val(std::move(v)) {};
        MalData(MalBoolean v) : val(v) {};
        MalData(MalInteger v) : val(v) {};
        MalData(MalKeyword v) : val(std::move(v)) {};
        MalData(MalSymbol v) : val(std::move(v)) {};
        MalData(MalString v) : val(std::move(v)) {};
        MalData(MalList v) : val(std::move(v)) {};
        MalData(MalVector v) : val(std::move(v)) {};
        MalData(MalMap v) : val(std::move(v)) {};
        MalData(MalAtom v) : val(std::move(v)) {};
        MalData(MalFn v) : val(std::move(v)) {};
        MalData(MalNativeFn v) : val(std::move(v)) {};
        MalData(const MalData& o) : val(o.val) {};
        MalData(MalData&& o) noexcept : val(std::move(o.val)) {};
        auto operator=(const MalData& o) -> MalData& = default;
        auto operator=(MalData&& o)  noexcept -> MalData& {
            std::swap(val, o.val);
            return *this;
        }

        auto operator<=>(const mal::MalData& right) const -> std::strong_ordering;
        inline auto operator==(const mal::MalData& right)const  { return (*this <=> right) == std::strong_ordering::equivalent; }
        inline auto operator!=(const mal::MalData& right) const { return (*this <=> right) != std::strong_ordering::equivalent; }
        inline auto operator<(const mal::MalData& right) const { return (*this <=> right) == std::strong_ordering::less; }
        inline auto operator>(const mal::MalData& right) const { return (*this <=> right) == std::strong_ordering::greater; }
        inline auto operator<=(const mal::MalData& right) const {
            auto cmp = (*this <=> right);
            return cmp == std::strong_ordering::less || cmp == std::strong_ordering::equivalent;
        }
        inline auto operator>=(const mal::MalData& right) const {
            auto cmp = (*this <=> right);
            return cmp == std::strong_ordering::greater || cmp == std::strong_ordering::equivalent;
        }
        [[nodiscard]] auto is_nil() const { return std::holds_alternative<MalNil>(val); }
        [[nodiscard]] auto is_int() const { return std::holds_alternative<MalInteger>(val); }
        [[nodiscard]] auto is_keyword() const { return std::holds_alternative<MalKeyword>(val); }
        [[nodiscard]] auto is_symbol() const { return std::holds_alternative<MalSymbol>(val); }
        [[nodiscard]] auto is_string() const { return std::holds_alternative<MalString>(val); }
        [[nodiscard]] auto is_list() const { return std::holds_alternative<MalList>(val); }
        [[nodiscard]] auto is_vec() const { return std::holds_alternative<MalVector>(val); }
        [[nodiscard]] auto is_map() const { return std::holds_alternative<MalMap>(val); }
        [[nodiscard]] auto is_native_fn() const { return std::holds_alternative<MalNativeFn>(val); }
        [[nodiscard]] auto is_user_fn() const { return std::holds_alternative<MalFn>(val); }
        [[nodiscard]] auto is_bool() const { return std::holds_alternative<MalBoolean>(val); }
        [[nodiscard]] auto is_atom() const { return std::holds_alternative<MalAtom>(val); }
        [[nodiscard]] auto is_falsey() const { return is_nil() || (*this) == MalBoolean{false}; }
        [[nodiscard]] auto is_truthy() const { return !is_falsey(); }

        [[nodiscard]] auto is_seq() const { return is_list() || is_vec(); }
        [[nodiscard]] auto is_coll() const { return is_map() || is_seq(); }
        [[nodiscard]] auto is_number() const { return is_int(); }
        [[nodiscard]] auto is_string_like() const { return is_string() || is_keyword() || is_symbol(); }
        [[nodiscard]] auto is_fn() const { return is_native_fn() || is_user_fn(); }
        [[nodiscard]] auto is_macro() const { return is_user_fn() && std::get<mal::MalFn>(val).is_macro; }
    };

    struct MalNs {
        std::string name;
        std::map<MalSymbol, MalData> mappings;
    };

    auto core_ns() -> MalNs;

    class MalEnv {
        std::map<MalSymbol, MalData> defined = {};
        std::shared_ptr<MalEnv> outer = nullptr;
    public:
        MalEnv();
        explicit MalEnv(std::shared_ptr<MalEnv> outer): outer(outer) {}
        MalEnv(std::shared_ptr<MalEnv> outer, std::map<MalSymbol, MalData> bindings): outer(outer), defined(bindings) {}

        std::map<MalSymbol, MalData> definitions() const {
            auto map = outer ? outer->definitions() : (decltype(defined)){};
            for (const auto& [key, val] : defined) {
                map[key] = val;
            }
            return map;
        }

        MalData defs() const {
            MalMap res = outer ? std::get<MalMap>(outer->defs().val) : MalMap{};
            for (const auto&[key, val] : defined) {
                res.val[key] = val;
            }
            return res;
        }

        MalEnv& root() {
            auto* e = this;
            while (e->outer != nullptr) {
                e = &(*outer);
            }
            return *e;
        }

        void mergeWith(const std::shared_ptr<MalEnv>& other) {
            for (const auto& [key, val] : other->definitions()) {
                defined[key] = val;
            }
        }

        class MalEnvEntry {
            MalEnv& env;
            MalSymbol key;
        public:
            MalEnvEntry(MalEnv& env, MalSymbol key) : env(env), key(std::move(key)) {}

            auto operator=(MalData data) -> MalData&{
                env.defined[key] = std::move(data);
                return env.defined[key];
            }

            operator MalData() const {
                if (env.defined.contains(key)) {
                    return env.defined[key];
                }
                else if (env.outer != nullptr) {
                    return (*env.outer)[key];
                }
                else {
                    throw std::runtime_error("Key '" + key.val + "' not found!");
                }
            }
        };

        auto operator[](const MalSymbol& key) -> MalEnvEntry{
            return MalEnvEntry{*this, key};
        }

        auto contains(const MalSymbol& key) const -> bool{
            if (defined.contains(key)) {
                return true;
            }
            else if (outer != nullptr) {
                return outer->contains(key);
            }
            else {
                return false;
            }
        }
    };

    auto READ(const std::string& str) -> mal::MalData;
    auto EVAL(std::shared_ptr<MalEnv> env, mal::MalData data) -> mal::MalData;
    auto PRINT(const mal::MalData& data, bool print_readably = true) -> std::string;
    auto rep(std::shared_ptr<MalEnv> env, std::string str, bool print_readably = true) -> std::string;

    auto hash(const mal::MalData& obj) -> size_t;
    auto quasiquote(std::shared_ptr<mal::MalEnv> env, mal::MalData ast) -> mal::MalData;
}

template <>
struct std::hash<mal::MalData>
{
    std::size_t operator()(const mal::MalData& k) const
    {
        return mal::hash(k);
    }
};