#pragma once
#include <source_location>

#define ZT_TYPE_OF(x) tryn::utl::type_of<x>()

#ifdef _MSC_VER
#define BEGIN_OFFSET 41
#define END_OFFSET 7
#endif

#ifdef __GNUC__
#define BEGIN_OFFSET 48
#define END_OFFSET 1
#endif

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

	    return {functionName.begin() + BEGIN_OFFSET, functionName.end() - END_OFFSET};;
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
