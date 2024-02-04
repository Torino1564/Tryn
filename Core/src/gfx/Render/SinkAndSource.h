#pragma once
#include <tuple>
#include <memory>
#include <string>
#include <array>
#include <concepts>
#include <vector>
#include <iostream>

#define ZT_DECLARE_EXPOSURES(x) decltype(Source(x)) source = Source(x)
#define ZT_DECLARE_DEPENDENCIES(x) decltype(Sink(x)) sink = Sink(x)

namespace tryn::gfx
{
	enum class Policy {
		ReadOnly,
		ReadWrite,
		WriteOnly
	};

	template<typename T, Policy P = Policy::ReadWrite>
	struct In
	{
		In(std::string name) : name(std::move(name)) {}
		using SysType = T;
		std::string name;
		Policy PolicyType = P;
	};

	class ISink
	{

	};

	template <typename... Dependencies>
	class Sink : public ISink
	{
	public:
		Sink() = delete;
		Sink(Dependencies&&... ins)
		{
			AddDependencies(std::move(std::forward_as_tuple(std::forward<Dependencies>(ins)...)));
		}
	private:
		template <int N = 0, typename... Dependencies>
		void AddDependencies(std::tuple<Dependencies&&...>&& container)
		{
			auto standalone = std::move(std::get<N>(container));
			namesAndPolicy.push_back({ std::move(standalone.name), std::move(standalone.PolicyType)});
			if constexpr (N < sizeof...(Dependencies) - 1)
			{
				AddDependencies<N + 1, Dependencies...>(std::move(container));
			}
		}

	public:
		template <typename T>
		std::shared_ptr<T> Get(const std::string& name)
		{
			return GetImpl_(name);
		}
		using DependencyTuple = typename std::tuple<std::shared_ptr<typename Dependencies::SysType>...>;
		std::vector<std::pair<std::string, Policy>> namesAndPolicy;
	private:
		template <unsigned N = 0, typename T>
		std::shared_ptr<T> GetImpl_(const std::string& name)
		{
			if constexpr (std::is_same_v<std::tuple_element_t<N, DependencyTuple>, T>)
			{
				if (namesAndPolicy[N] != name)
				{
					continue;
				}
				return std::get<N>(dependencyTuple);
			}
			if constexpr (N < std::tuple_size_v<DependencyTuple> - 1)
			{
				return GetImpl_<N + 1>(name);
			}
		}
		DependencyTuple dependencyTuple;
	};

	class ISource
	{

	};

	template<typename T>
	struct Out
	{
		Out(std::string name) : name(std::move(name)) {}
		using SysType = T;
		std::string name;
	};

	template <typename... Exposures>
	class Source : public ISource
	{
	public:
		Source() = delete;
		Source(Exposures&&... outs)
		{
			AddExposures(std::move(std::forward_as_tuple(std::forward<Exposures>(outs)...)));
		}
	private:
		template <int N = 0, typename... Exposures>
		void AddExposures(std::tuple<Exposures&&...>&& container)
		{
			auto standalone = std::move(std::get<N>(container));
			names.push_back(std::move(standalone.name));
			if constexpr (N < sizeof...(Exposures) - 1)
			{
				AddExposures<N + 1, Exposures...>(std::move(container));
			}
		}

	public:
		using ExposureTuple = typename std::tuple<std::shared_ptr<typename Exposures::SysType>...>;
		ExposureTuple exposureTuple;
		std::vector<std::string> names;
	};
}