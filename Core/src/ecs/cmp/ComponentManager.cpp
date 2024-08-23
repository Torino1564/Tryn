#include "TrynPCH.h"
#include "ComponentManager_def.h"
#include <Core/src/ecs/Entity.h>
#include "ActiveComponent.h"
#include "Core/src/ecs/Archetype.h"

namespace tryn::ecs
{
	ECS::ECS()
		:
		componentManager(ComponentManager::Get()), archetypeManager(ArchetypeManager::Get())
	{}

	void Archetype::InitializeUUID()
	{
		UUID = ECS::Get().archetypeManager.ResolveUUID();
	}

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

	template ReturnType<ComponentInfo::Index>::T GetComponentInfo<ComponentInfo::Index>(utl::UUID_t);
	template ReturnType<ComponentInfo::Name>::T GetComponentInfo<ComponentInfo::Name>(utl::UUID_t);
	template ReturnType<ComponentInfo::Size>::T GetComponentInfo<ComponentInfo::Size>(utl::UUID_t);

}

