#pragma once
#include <span>
#include <string>
#include <algorithm>

namespace tryn::utl
{
	template <unsigned int N>
    struct StaticString
    {
        constexpr StaticString(const char(&str)[N])
        {
            std::copy(str, N, v);
        }
        template <unsigned int M = N>
        constexpr StaticString Make(std::string_view view)
        {
            return StaticString<M>(view.data());
        }
        constexpr StaticString(std::span<char, N> view)
        {
            std::copy_n(view.data(), N, v);
        }
        static constexpr unsigned int n = N;
        char v[N];
    };

    template <StaticString TypeName, auto Tag = [] {} >
    constexpr auto TextType = [] {
        return TypeName.v;
        };

    template <unsigned int N, auto Tag = [] {} >
    constexpr auto IntType = [] {
        return N;
        };
}