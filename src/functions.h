#pragma once

#include <functional>

namespace fn {
    template<typename T, typename... U>
    size_t get_address(std::function<T(U...)> f) {
        typedef T(fnType)(U...);
        fnType **fnPointer = f.template target<fnType *>();
        return (size_t) *fnPointer;
    }
}
