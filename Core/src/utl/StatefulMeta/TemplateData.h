#pragma once

namespace tryn::utl
{
	template <unsigned int N>
    struct StaticString
    {
        constexpr StaticString(const char(&str)[N])
        {
            std::copy_n(str, N, v);
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