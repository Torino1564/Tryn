#pragma once
#include <tuple>

namespace tryn::utl
{
	template <typename T, typename Tuple>
	struct has_type;

	template <typename T>
	struct has_type<T, std::tuple<>> : std::false_type {};

	template <typename T, typename U, typename... Ts>
	struct has_type<T, std::tuple<U, Ts...>> : has_type<T, std::tuple<Ts...>> {};

	template <typename T, typename... Ts>
	struct has_type<T, std::tuple<T, Ts...>> : std::true_type {};

	template <typename T, typename Tuple>
	using tuple_contains_type = typename has_type<T, Tuple>::type;

	template <typename T, typename Tuple>
	static constexpr bool tuple_contains_type_v = tuple_contains_type<T, Tuple>::value;

	template <typename T, typename Tuple, unsigned N = 0>
	consteval unsigned GetTypeIndex()
	{
		static_assert(N < std::tuple_size_v<Tuple>, "Type not found in tuple");

		if constexpr (std::is_same_v<T, std::tuple_element_t<N, Tuple>>)
		{
			return N;
		}
		else
		{
			return GetTypeIndex<T, Tuple, N + 1>();
		}
	}

	template <typename T>
	struct ArgTuple;

	template <typename Func, typename... Args>
	struct ArgTuple<Func(Args...)>
	{
		using t = std::tuple<Args...>;
	};

	template <typename T>
	struct MethodArgTupleMinusFirst;

	template <typename Func, typename Gfx, typename... Args>
	struct MethodArgTupleMinusFirst<Func(*)(Gfx, Args...)>
	{
		using t = std::tuple<Args...>;
	};
}