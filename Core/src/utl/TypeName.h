#pragma once

#include <array>
#include <string>
#include <algorithm>
#include <source_location>
#include <memory>

#define ZT_TYPE_OF(x) tryn::utl::type_of<x>()

#ifdef _MSC_VER
#define BEGIN_OFFSET 95
#define END_OFFSET 7
#endif

#if  __clang__
#define BEGIN_OFFSET 33
#define END_OFFSET 1
#endif

namespace tryn::utl
{
	template <auto Data>
	consteval const auto& make_static()
	{
		return Data;
	}

	struct oversized_array
	{
		std::array<char, 1 * 1024> data{};
		std::size_t size;
	};

	constexpr auto to_oversized_array(const std::string& str)
	{
		oversized_array result;
		std::copy(str.begin(), str.end(), result.data.begin());
		result.size = str.size();
		return result;
	}

	consteval auto to_right_sized_array(auto callable)
	{
		constexpr auto oversized = to_oversized_array(callable());
		std::array<char, oversized.size> result;
		std::copy(oversized.data.begin(), std::next(oversized.data.begin(), oversized.size), result.begin());
		return result;
	}

	consteval auto to_string_view(auto callable) -> std::string_view
	{
		constexpr auto& static_data = make_static<to_right_sized_array(callable)>();
		return std::string_view{static_data.begin(), static_data.end()};
	}

	template <typename T>
	consteval auto func_name()
	{
	    const auto& loc = std::source_location::current();
	    std::string_view retval = loc.function_name();

		return retval;
	}

    template <typename T>
    struct GetTypeNameString
    {
		constexpr std::string operator()()
		{
			auto extraOffset = 0;
            #ifdef _MSC_VER
			if constexpr (std::is_class_v<T>)
			{
				extraOffset += 6;
			}
            #endif
			std::string functionName = func_name<T>().data();

			return { functionName.begin() + BEGIN_OFFSET + extraOffset, functionName.end() - END_OFFSET };
		}
    };

    template <typename T>
    consteval std::string_view type_of()
    {
        return to_string_view(GetTypeNameString<T>());
    }

    template <typename T>
    constexpr std::string_view type_of(T&& arg)
    {
        return to_string_view(GetTypeNameString<T>());
    }

	template <> constexpr std::string_view type_of<bool>()
    {
		return "bool";
    }

	static constexpr auto name = type_of<float>();
}