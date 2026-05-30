#pragma once
#include <ranges>

namespace tryn
{
    namespace utl {
        template <typename R>
        auto enumerate(R&& range) {
            return std::views::zip(std::views::iota(0), std::forward<R>(range));
        }
    }
}

