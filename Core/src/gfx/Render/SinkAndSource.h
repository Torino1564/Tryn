#pragma once
#include <any>
#include <tuple>
#include <memory>
#include <string>
#include <array>
#include <vector>
#include <Core/src/utl/Assert.h>
#include <bitset>
#include <Core/src/utl/String.h>
#include <Core/src/log/Log.h>

#include "Core/src/utl/StringHasher.h"
#include <any>

namespace tryn::gfx
{
	class Source;
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


	class Sink 
	{
	public:
		template <typename T>
		void AddDependency(const std::string& dependency);
		void Bind(Source& source, const std::string& exposure, const std::string& dependency, std::optional<utl::UUID_t> uuid = std::nullopt);

		template <typename T>
		std::shared_ptr<T> Get(const std::string& dependency);
	private:
		struct Entry
		{
			std::string name;
			utl::UUID_t uuid;
			Source* pSource;
			uint16_t exposureIndex;
		};

		std::vector<Entry> data;
	};

	template<typename T>
	struct Out
	{
		Out(std::string name) : name(std::move(name)) {}
		using SysType = T;
		std::string name;
	};

	class Source
	{
	public:
		friend class Sink;
		template <typename T>
		void AddExposure(const std::string& name);
		template <typename T>
		void Set(const std::shared_ptr<T>& pResource, const std::string& exposureName);
		std::any& Get(uint16_t index);
		bool IsNonConstBounded(const std::string& exposureName)
		{
		}

		template <typename T>
		const std::shared_ptr<T>& Get(const std::string& exposureName);
		

		enum struct Type
		{
			Const,
			NonConst
		};

		struct Entry
		{
			std::string name;
			utl::UUID_t uuid;
			std::any resource;
			std::vector<std::pair<Sink*, Type>> bindings;
		};

		// 0 means unbounded, 1 is bounded
		std::vector<Entry> data;
	};

	template <typename T>
	void Sink::AddDependency(const std::string& dependency)
	{
		// Todo: assert uniqueness
		data.emplace_back(dependency, ZT_TYPE_UUID(T), nullptr, 0);
	}

	template <typename T>
	std::shared_ptr<T> Sink::Get(const std::string& dependency)
	{
		auto it = std::ranges::find_if(data, [&](const Entry& entry)
			{
				return entry.name == dependency;
			});
		trynass(it != data.end()).msg(L"Could not find the dependency: " + utl::ToWide(dependency));
		trynass(it->uuid == ZT_TYPE_UUID(T)).msg(L"Missmatch between dependency type and requested type");

		return std::any_cast<std::shared_ptr<T>>(it->pSource->Get(it->exposureIndex));
	}

	template <typename T>
	void Source::AddExposure(const std::string& name)
	{
		// Todo: assert uniqueness
		data.emplace_back(name, ZT_TYPE_UUID(T), {}, {});
	}

	template <typename T>
	void Source::Set(const std::shared_ptr<T>& pResource, const std::string& exposureName)
	{
		// assert no duplicate exposures:
		const auto it = std::ranges::find_if(data, [&](const auto& tuple)
			{
				return tuple.first == exposureName;
			});
		trynass(it == data.end());

		// Append new element
		data.emplace_back({
			exposureName,
			ZT_TYPE_UUID(T),
			pResource,
			{}
			});
	}

	template <typename T>
	const std::shared_ptr<T>& Source::Get(const std::string& exposureName)
	{
		const auto it = std::ranges::find_if(data, [&](const Entry& entry)
			{
				return (
					entry.name == exposureName &&
					entry.uuid == ZT_TYPE_UUID(T)
					);
			});
		trynass(it != data.end()).msg(L"Failed finding dependency: " + utl::ToWide(exposureName));

		return std::any_cast<T>(it->resource);
	}
}
