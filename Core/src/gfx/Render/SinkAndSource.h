#pragma once
#include <tuple>
#include <memory>
#include <string>
#include <array>
#include <concepts>
#include <vector>
#include <iostream>
#include <Core/src/utl/Assert.h>
#include <bitset>
#include <Core/src/utl/String.h>

#define ZT_DECLARE_EXPOSURES(x) decltype(Source(x)) source = Source(x)
#define ZT_DECLARE_DEPENDENCIES(x) decltype(Sink(x)) sink = Sink(x)

namespace tryn::gfx
{
	class IBindable;

	enum class Policy {
		ReadOnly,
		ReadWrite,
		WriteOnly,
		Barrier,
	};

	template<typename T, Policy P = Policy::ReadWrite>
	struct In
	{
		In(std::string name) : name(std::move(name)) {}
		using SysType = T;
		std::string name;
		Policy PolicyType = P;
	};

	template <typename... Args>
	class Source;

	class ISink
	{
		template <typename... Args>
		friend class Source;
	public:
		virtual bool IsBounded(const std::string& dependencyName) = 0;
	protected:
		virtual void Accept(const std::string& dependencyName, std::shared_ptr<IBindable>*& pDependency) = 0;
		std::vector<std::pair<std::string, Policy>> namesAndPolicy;
	};

	template <typename... Dependencies>
	class Sink : public ISink
	{
	public:
		virtual ~Sink() = default;
		Sink() = delete;
		Sink(Dependencies&&... ins)
		{
			AddDependencies(std::move(std::forward_as_tuple(std::forward<Dependencies>(ins)...)));
		}
		void Accept(const std::string& dependencyName, std::shared_ptr<IBindable>*& pDependency) override
		{
			AcceptImpl_(dependencyName, pDependency);
		}
		bool IsBounded(const std::string& dependencyName) override
		{
			for (int i = 0; i < namesAndPolicy.size(); i++)
			{
				if (namesAndPolicy[i].first == dependencyName)
				{
					return bound[i];
				}
			}
			return false;
		}
	private:
		template <unsigned N = 0>
		void AcceptImpl_(const std::string& dependencyName, std::shared_ptr<IBindable>*& pDependency)
		{
			// check if this is the correct dependency index
			if (namesAndPolicy[N].first == dependencyName)
			{
				// down cast shared_ptr reference to concrete type
				using DependencyType = typename std::tuple_element_t<N, DependencyTypeTuple>;
				auto ptr = reinterpret_cast<std::shared_ptr<DependencyType>**>(&pDependency);
				trynass(ptr).msg(L"Invalid type passed to the Sink Accept function!").ex();
				std::get<N>(dependencyTuple) = ptr;
				// assert not bounded
				trynass(!bound[N]).msg(utl::ToWide(std::format("The dependency [{}] already is bounded!", dependencyName))).ex();
				// set as bound
				bound.set(N, true);
				return;
			}
			if constexpr (N < std::tuple_size_v<DependencyTuple> - 1)
			{
				return AcceptImpl_<N + 1>(dependencyName, pDependency);
			}
			trynchk_fail.msg(utl::ToWide(std::format("Did not find the dependency [{}]!", dependencyName))).ex();
		}
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
		std::shared_ptr<T>& Get(const std::string& name)
		{
			return GetImpl_<0,T>(name);
		}
		using DependencyTuple = typename std::tuple<std::shared_ptr<typename Dependencies::SysType>**...>;
		using DependencyTypeTuple = typename std::tuple<typename Dependencies::SysType...>;

	private:
		template <unsigned N = 0, typename T>
		std::shared_ptr<T>& GetImpl_(const std::string& name)
		{
			if constexpr (std::is_same_v<std::tuple_element_t<N, DependencyTypeTuple>, T>)
			{
				if (namesAndPolicy[N].first == name)
				{
					return **std::get<N>(dependencyTuple);
				}
			}
			if constexpr (N < std::tuple_size_v<DependencyTuple> - 1)
			{
				return GetImpl_<N + 1, T>(name);
			}
			trynchk_fail.msg(L"Did not find the required dependency!").ex();
		}

	public:
		DependencyTuple& GetDependencyTuple()
		{
			return dependencyTuple;
		}

	private:

		DependencyTuple dependencyTuple;
		std::bitset<sizeof...(Dependencies)> bound;

	};

	class ISource
	{
	public:
		virtual bool IsBounded(const std::string& exposureName) = 0;
		virtual void Bind(ISink& sink, const std::string& exposureName, const std::string& dependencyName) = 0;
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
		~Source() = default;
		Source() = delete;
		Source(Exposures&&... outs)
		{
			AddExposures(std::move(std::forward_as_tuple(std::forward<Exposures>(outs)...)));
		}
		void Bind(ISink& sink, const std::string& exposureName, const std::string& dependencyName) override
		{
			// get the exposure ptr
			auto [ppExposure, index] = GetExposurePtrAndIndex(exposureName);
			// assert not set
			trynchk(!bound[index]).msg(utl::ToWide(std::format("The exposure [{}] already is bounded! Overriding binding", exposureName))).lvl(log::Level::Warn);
			// set exposure as bounded
			bound.set(index);
			// send the ptr to the sink
			sink.Accept(dependencyName, *ppExposure);
		}
		template <typename T>
		void Set(std::shared_ptr<T>& pResource, const std::string& exposureName)
		{
			SetImpl_(pResource, exposureName);
		}
		virtual bool IsBounded(const std::string& exposureName) override
		{
			for (int i = 0; i < names.size(); i++)
			{
				if (names[i] == exposureName)
				{
					return bound[i];
				}
			}
			trynchk_fail.msg(L"Invalid exposure name!");
			return false;
		}
	private:
		template <unsigned N = 0, typename T>
		void SetImpl_(std::shared_ptr<T>& pResource, const std::string& exposureName)
		{
			if constexpr (std::is_same_v<typename std::remove_pointer_t<std::tuple_element_t<N, ExposureTuple>>::element_type, T>)
			{
				if (names[N] == exposureName)
				{
					auto& pExposure = std::get<N>(exposureTuple);
					pExposure = &pResource;
					return;
				}
			}
			if constexpr (N < std::tuple_size_v<ExposureTuple> -1)
			{
				return SetImpl_<N + 1>(pResource, exposureName);
			}
			trynchk_fail.msg(L"No exposure was found for that type/name combination!").ex();
		} 
		auto GetExposurePtrAndIndex(const std::string& exposureName)
		{
			return GetExposurePtrAndIndexImpl_(exposureName);
		}
		template <unsigned N = 0>
		std::pair<std::shared_ptr<IBindable>**, unsigned> GetExposurePtrAndIndexImpl_(const std::string& exposureName)
		{
			if (exposureName == names[N])
			{
				return { reinterpret_cast<std::shared_ptr<IBindable>**>(&std::get<N>(exposureTuple)), N };
			}
			if constexpr (N < std::tuple_size_v<ExposureTuple> -1)
			{
				return GetExposurePtrAndIndexImpl_<N + 1>(exposureName);
			}
			static std::shared_ptr<IBindable> fallback = nullptr;
			static auto pFallback = &fallback;
			return { &pFallback, 0u };
		}
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
		using ExposureTuple = typename std::tuple<std::shared_ptr<typename Exposures::SysType>*...>;
		ExposureTuple exposureTuple;
		// 0 means unbounded, 1 is bounded
		std::bitset<sizeof...(Exposures)> bound;
		std::vector<std::string> names;
	};
	
	template <typename... Exposures>
	std::unique_ptr<Source<Exposures...>> MakeUniqueSource(Exposures&&... outs)
	{
		return std::make_unique<Source<Exposures...>>(std::move(Source(std::forward<Exposures>(outs)...)));
	}

	template <typename... Dependencies>
	std::unique_ptr<Sink<Dependencies...>> MakeUniqueSink(Dependencies&&... ins)
	{
		return std::make_unique<Sink<Dependencies...>>(std::move(Sink(std::forward<Dependencies>(ins)...)));
	}
}