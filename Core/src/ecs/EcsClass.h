#pragma once
#include <string>
#include <type_traits>
#include "Core/src/utl/StatefulMeta/TemplateData.h"

namespace tryn::ecs
{
	class ComponentManager;

	template <typename T, utl::StaticString Name>
	class Component;

	template <typename T>
	concept ImplementsUUID = requires
	{
		std::convertible_to<decltype(T::UUID), std::string>;
	};

	template <typename T>
	concept ImplementsSRD = requires
	{
		typename T::SubresourceData;
	};

	template <typename T>
	concept ValidComponent = ImplementsSRD<T> && ImplementsUUID<T>;

	enum class AccessMode
	{
		ReadWrite,
		ReadOnly,
		WriteOnly,
		Discard
	};

	template <typename T>
	concept ValidComponentWithAccessMode =  ValidComponent<typename T::ComponentType> and requires
	{
		{T::accessMode} -> std::convertible_to<AccessMode>;
	};


	template <ValidComponent C>
	struct ReadOnly
	{
		using ComponentType = C;
		static constexpr auto accessMode = AccessMode::ReadOnly;
	};

	template <ValidComponent C>
	struct ReadWrite
	{
		using ComponentType = C;
		static constexpr auto accessMode = AccessMode::ReadWrite;
	};

	template <ValidComponent C>
	struct WriteOnly
	{
		using ComponentType = C;
		static constexpr auto accessMode = AccessMode::WriteOnly;
	};

	template <ValidComponent C>
	struct Discard
	{
		using ComponentType = C;
		static constexpr auto accessMode = AccessMode::Discard;
	};

	class ComponentManager;
	class ArchetypeManager;

	using ComponentSize = typename std::size_t;

	class ECS
	{
	public:
		static ECS& Get()
		{
			static ECS ecs;
			return ecs;
		}
		ComponentManager& componentManager;
		ArchetypeManager& archetypeManager;
		mem::ArenaAllocator<> allocator;
	private:
		ECS();
	};
}
