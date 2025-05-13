#include "TrynPCH.h"
#include "ComponentManager.h"

#include <Core/src/ecs/cmp/Components.h>
#include <Core/src/gfx/Model/Model.h>

#include "UpdateJITBufferComponent.h"

namespace tryn::ecs
{
	uint16_t ComponentManager::ComponentCount() const
	{
		return componentCount;
	}

	const ComponentWrapper& ComponentManager::Wrapper(const utl::UUID_t componentUUID) const
	{
		return componentWrappers.at(componentUUID);
	}

	uint16_t ComponentManager::NextFreeAndIncrement()
	{
		return componentCount++;
	}

	ComponentManager::ComponentManager(const ECS* pEcs)
		: pEcs(pEcs)
	{
		// Register default components
		RegisterComponent<PositionComponent>();
		RegisterComponent<VelocityComponent>();
		RegisterComponent<AccelerationComponent>();
		RegisterComponent<ScaleComponent>();
		RegisterComponent<ActiveComponent>();
		RegisterComponent<AnimatedComponent>();
		RegisterComponent<BoneTransformsComponent>();
		RegisterComponent<PointLightComponent>();
		RegisterComponent<ModelComponent>();
		RegisterComponent<RotationComponent>();
		RegisterComponent<InstancedModelChildComponent>();
		RegisterComponent<InstancedModelParentComponent>();
		RegisterComponent<TransformComponent>();
		RegisterComponent<UpdateJITBufferComponent>();
	}
}

