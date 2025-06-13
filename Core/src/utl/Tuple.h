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

	template <typename T, typename TupleOfRegisters, unsigned N = 0>
	consteval unsigned GetTypeIndexFromTupleOfRegister()
	{
		static_assert(N < std::tuple_size_v<TupleOfRegisters>, "Type not found in tuple");

		if constexpr (std::is_same_v<T, typename std::tuple_element_t<N, TupleOfRegisters>::Bindable_t>)
		{
			return N;
		}
		else
		{
			return GetTypeIndexFromTupleOfRegister<T, TupleOfRegisters, N + 1>();
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

	template <typename Tuple>
	struct DecayTuple;

	template <typename... Ts>
	struct DecayTuple<std::tuple<Ts...>> {
		using type = std::tuple<std::decay_t<Ts>...>;
	};

	template <typename T>
	using DecayTuple_t = typename DecayTuple<T>::type;

	template <typename Tuple>
	struct RemoveCVRefTuple;

	template <typename... Ts>
	struct RemoveCVRefTuple<std::tuple<Ts...>> {
		using type = std::tuple<std::remove_cvref_t<Ts>...>;
	};

	template <typename T>
	using RemoveCVRefTuple_t = typename RemoveCVRefTuple<T>::type;

	template <typename Tuple>
	struct ConstRefTuple;

	template <typename... Ts>
	struct ConstRefTuple<std::tuple<Ts...>> {
		using type = std::tuple<const std::remove_cvref_t<Ts>&...>;
	};

	template <typename T>
	using ConstRefTuple_t = typename ConstRefTuple<T>::type;

	template <typename T, typename Tuple>
	struct is_constructible_from_tuple;

	template <typename T, typename... Args>
	struct is_constructible_from_tuple<T, std::tuple<Args...>>
		: std::is_constructible<T, Args...> {};

	template <typename T, typename Tuple>
	static constexpr bool is_constructible_from_tuple_v = is_constructible_from_tuple<T, Tuple>::type::value;
}