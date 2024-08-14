#pragma once
#include <source_location>

namespace tryn::utl
{
	template <typename T>
	consteval auto func_name() {
	    const auto& loc = std::source_location::current();
	    return loc.function_name();
	}

	template <typename T>
	consteval std::string_view type_of_impl_() {
	    constexpr std::string_view functionName = func_name<T>();

	    return {functionName.begin() + 37, functionName.end() - 1};
	}

	template <typename T>
	constexpr auto type_of(T&& arg) {
	    return type_of_impl_<decltype(arg)>();
	}

	template <typename T>
	constexpr auto type_of() {
	    return type_of_impl_<T>();
	}

	static constexpr auto name = type_of<float>();
}
