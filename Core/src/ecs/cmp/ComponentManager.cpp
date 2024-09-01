#include "TrynPCH.h"
#include "ComponentManager_def.h"
#include <Core/src/ecs/Entity.h>
#include "ActiveComponent.h"
#include "Core/src/ecs/Archetype.h"
#include "Core/src/ecs/sys/AnimationSystem.h"
#include <Core/src/ecs/sys/SystemManager.h>

namespace tryn::ecs
{
	template <ComponentInfo Info>
	typename ReturnType<Info>::T GetComponentInfo(const utl::UUID_t componentUUID)
	{
		auto& map = ComponentManager::ComponentMap();
		auto& [pComponent, index] = map[componentUUID];

		if constexpr (Info == ComponentInfo::Name)
		{
			return pComponent->Name();
		}
		else if constexpr (Info == ComponentInfo::Size)
		{
			return pComponent->Size();
		}
		else if constexpr (Info == ComponentInfo::Index)
		{
			return index;
		}
	}

	unsigned int ComponentManager::NextFreeAndIncrement()
	{
		static unsigned int componentCount = 0;
		return componentCount++;
	}

	std::unordered_map<utl::UUID_t, std::pair<std::unique_ptr<IComponent>, unsigned int>>& ComponentManager::
	ComponentMap()
	{
		static std::unordered_map<utl::UUID_t, std::pair<std::unique_ptr<IComponent>, unsigned int>> componentMap;
		return componentMap;
	}

	std::vector<utl::UUID_t>& ComponentManager::ComponentVector()
	{
		static std::vector<utl::UUID_t> componentVector;
		return componentVector;
	}

	ComponentManager& ComponentManager::Get()
	{
		static ComponentManager singleton(nullptr);
		return singleton;
	}

	size_t ComponentManager::GetComponentCount()
	{
		return ComponentMap().size();
	}

	ComponentManager::ComponentManager(const ECS* pEcs)
		: pEcs(pEcs)
	{
	}

	template ReturnType<ComponentInfo::Index>::T GetComponentInfo<ComponentInfo::Index>(utl::UUID_t);
	template ReturnType<ComponentInfo::Name>::T GetComponentInfo<ComponentInfo::Name>(utl::UUID_t);
	template ReturnType<ComponentInfo::Size>::T GetComponentInfo<ComponentInfo::Size>(utl::UUID_t);

}

