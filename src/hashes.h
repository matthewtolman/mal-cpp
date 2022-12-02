#pragma once

#include <cstdlib>
#include <unordered_map>

namespace hashes {
    inline size_t hash_combine(size_t seed) { return seed; }

    template <typename T, typename... Rest>
    inline size_t hash_combine(std::size_t seed, const T& v, Rest... rest) {
        std::hash<T> hash{};
        return hash_combine(seed ^ hash(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2), rest...);
    }
}
