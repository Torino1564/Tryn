
#include "EcsClass.h"
#include <Core/src/ecs/cmp/ComponentManager.h>
#include <Core/src/ecs/sys/SystemManager.h>

#include "Core/src/app/App.h"

namespace tryn::ecs
{
	ECS::ECS(const app::App* pApp)
	:
		pApp(pApp), pComponentManager(std::make_unique<ComponentManager>(this)), pArchetypeManager(std::make_unique<ArchetypeManager>(this)), pSystemManager(std::make_unique<SystemManager>(this))
	{}

	ECS::~ECS() = default;

	void ECS::WipeAllocator()
	{
		allocator.Wipe();
	}

	const gfx::IGraphics& ECS::Gfx() const
	{
		return pApp->Gfx();
	}

	const mem::ArenaAllocator<>& ECS::GetAllocator() const
	{
		return allocator;
	}

	mem::ArenaAllocator<>& ECS::GetAllocator()
	{
		return allocator;
	}

	const ComponentManager& ECS::GetComponentManager() const
	{
		return *pComponentManager;
	}

	ComponentManager& ECS::GetComponentManager()
	{
		return *pComponentManager;
	}

	const ArchetypeManager& ECS::GetArchetypeManager() const
	{
		return *pArchetypeManager;
	}

	ArchetypeManager& ECS::GetArchetypeManager()
	{
		return *pArchetypeManager;
	}

	const SystemManager& ECS::GetSystemManager() const
	{
		return *pSystemManager;
	}

	SystemManager& ECS::GetSystemManager()
	{
		return *pSystemManager;
	}

	void ECS::ExecuteSystems() const
	{
		pSystemManager->ExecuteSystems();
	}
}
