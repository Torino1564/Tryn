#pragma once
#include <type_traits>
#include "Core/src/utl/StatefulMeta/TemplateData.h"
#include <Core/src/mem/ArenaAllocator.h>

#include "Core/src/utl/StringHasher.h"

namespace tryn::gfx
{
	class IGraphics;
}

namespace tryn::app
{
	class App;
}

namespace tryn::ecs
{
	class ComponentManager;

	template <typename T, utl::StaticString>
	class Component;

	template <typename T>
	concept ImplementsUUID = requires
	{
		std::convertible_to<decltype(T::UUID), utl::UUID_t>;
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
	concept ValidComponentWithAccessMode =  ValidComponent<typename T::ComponentType> or requires
	{
		{T::accessMode} -> std::convertible_to<AccessMode>;
		ValidComponent<typename T::ComponentType>;
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
	class SystemManager;

	using ComponentSize = typename std::size_t;

	class ECS
	{
	public:
		ECS(const app::App* pApp);
		~ECS();
		static ECS& Get()
		{
			static ECS ecs(nullptr);
			return ecs;
		}
		void WipeAllocator();
		const gfx::IGraphics& Gfx() const;
		const mem::ArenaAllocator<>& GetAllocator() const;
		mem::ArenaAllocator<>& GetAllocator();
		const ComponentManager& GetComponentManager() const;
		const ArchetypeManager& GetArchetypeManager() const;
		ArchetypeManager& GetArchetypeManager();
		const SystemManager& GetSystemManager() const;
	private:
		const app::App* pApp;
		std::unique_ptr<ComponentManager> pComponentManager;
		std::unique_ptr<ArchetypeManager> pArchetypeManager;
		std::unique_ptr<SystemManager> pSystemManager;
		mem::ArenaAllocator<> allocator;
	};
}
