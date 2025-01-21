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

	enum class AccessMode
	{
		ReadWrite,
		ReadOnly,
		WriteOnly,
		Discard
	};


	template <typename C>
	struct ReadOnly
	{
		using Component = C;
		static constexpr auto accessMode = AccessMode::ReadOnly;
	};

	template <typename C>
	struct ReadWrite
	{
		using Component = C;
		static constexpr auto accessMode = AccessMode::ReadWrite;
	};

	template <typename C>
	struct WriteOnly
	{
		using Component = C;
		static constexpr auto accessMode = AccessMode::WriteOnly;
	};

	template <typename C>
	struct Discard
	{
		using Component = C;
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
		void WipeAllocator();
		const gfx::IGraphics& Gfx() const;
		const mem::ArenaAllocator<>& GetAllocator() const;
		mem::ArenaAllocator<>& GetAllocator();
		const ComponentManager& GetComponentManager() const;
		ComponentManager& GetComponentManager() ;
		const ArchetypeManager& GetArchetypeManager() const;
		ArchetypeManager& GetArchetypeManager();
		const SystemManager& GetSystemManager() const;
		SystemManager& GetSystemManager();
	private:
		const app::App* pApp;
		std::unique_ptr<ComponentManager> pComponentManager;
		std::unique_ptr<ArchetypeManager> pArchetypeManager;
		std::unique_ptr<SystemManager> pSystemManager;
		mem::ArenaAllocator<> allocator;
	};
}
