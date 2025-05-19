#pragma once
#include <Core/src/utl/StringHasher.h>

namespace tryn::ecs
{
	struct PositionComponent;
}

namespace tryn ::ecs
{
	using ArchetypeID = uint32_t;

	template <typename T, typename Tuple>
	struct IsInTuple : std::false_type {};

	template <typename T>
	struct IsInTuple<T, std::tuple<>> : std::false_type {};

	template <typename T, typename U, typename... Rest>
	struct IsInTuple<T, std::tuple<U, Rest...>>
		: std::conditional_t<std::is_same_v<T, U>, std::true_type, IsInTuple<T, std::tuple<Rest...>>> {
	};

	template <typename T, typename Tuple>
	struct AppendUnique;

	template <typename T, typename... Ts>
	struct AppendUnique<T, std::tuple<Ts...>> {
		using type = std::conditional_t<
			IsInTuple<T, std::tuple<Ts...>>::value,
			std::tuple<Ts...>,
			std::tuple<Ts..., T>
		>;
	};

	template <typename TupleA, typename TupleB>
	struct UniqueTupleUnion;

	template <typename... As, typename... Bs>
	struct UniqueTupleUnion<std::tuple<As...>, std::tuple<Bs...>> {
	private:
		template <typename Tuple, typename... Ts>
		struct Merge;

		template <typename Tuple>
		struct Merge<Tuple> {
			using type = Tuple;
		};

		template <typename Tuple, typename T, typename... Ts>
		struct Merge<Tuple, T, Ts...> {
			using NewTuple = typename AppendUnique<T, Tuple>::type;
			using type = typename Merge<NewTuple, Ts...>::type;
		};

	public:
		using type = typename Merge<std::tuple<As...>, Bs...>::type;
	};

	template <typename Test, typename... Components>
		//requires std::predicate<Test, const std::tuple<Components...>&>
	struct Logic
	{
		using T = std::tuple<Components...>;

		constexpr bool operator()(const std::tuple<Components...>& cs)
		{
			static constexpr Test test{};
			return test(cs);
		}
	};

	template <typename Tuple, typename Test>
	struct TupleToLogic;

	template <typename Test, typename... Args>
	struct TupleToLogic<std::tuple<Args...>, Test> {
		using type = Logic<Test, Args...>;
	};

	
	template <typename Logic1, typename Logic2>
	struct And
	{
		struct Operator
		{
			constexpr bool operator()(const typename Logic1::T& t1, const typename Logic2::T& t2)
			{
				return Logic1::operator()(t1) && Logic2::operator()(t2);
			}
		};
		using UniqueTuple = UniqueTupleUnion<typename Logic1::T, typename Logic2::T>;
		using type = typename TupleToLogic<UniqueTuple, Operator>::type;
	};

	template <typename Logic1, typename Logic2>
	struct Or
	{
		struct Operator
		{
			constexpr bool operator()(const typename Logic1::T& t1, const typename Logic2::T& t2)
			{
				return Logic1::operator()(t1) || Logic2::operator()(t2);
			}
		};
		using UniqueTuple = UniqueTupleUnion<typename Logic1::T, typename Logic2::T>;
		using type = typename TupleToLogic<UniqueTuple, Operator>::type;
	};

	template <typename... Components>
	class Query
	{
	public:

		consteval Query() = default;

		template<typename... Cs>
		friend constexpr Query<Cs...> Select();

		constexpr Query& From(const std::vector<ArchetypeID>& archetypes_in = {});

		template<typename Logic>
		constexpr Query& Where();


	//private:
		enum struct Previous
		{
			None,
			Select,
			From,
			Condition
		};
		std::vector<ArchetypeID> archetypes;
		std::array<uint32_t, sizeof...(Components)> components;
		bool (*pLogic)(const std::tuple<Components...>&) = nullptr;
		Previous state = Previous::None;
	};

	template <typename ... Components>
	constexpr Query<Components...>& Query<Components...>::From(const std::vector<ArchetypeID>& archetypes_in)
	{
		for (auto archetype : archetypes_in)
		{
			archetypes.push_back(archetype);
		}
		state = Previous::From;

		return *this;
	}

	template <typename ... Components>
	template <typename Logic>
	constexpr Query<Components...>& Query<Components...>::Where()
	{
		pLogic = &Logic::operator();
	}


	template <typename... Components>
	static constexpr Query<Components...> Select()
	{
		Query<Components...> retval;
		retval.components = { ZT_TYPE_UUID(Components)... };
		retval.state = Query<Components...>::Previous::Select;
		return retval;
	}
}
