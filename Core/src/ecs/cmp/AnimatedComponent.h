#pragma once
#include "ComponentManager.h"
#include <Core/src/gfx/Animation/BonedMesh.h>

namespace tryn::ecs::cmp
{
	ZT_DEFINE_COMPONENT(AnimatedComponent)
	{
		ZT_COMPONENT_FIELDS(
			double time;
			uint32_t previousKey;
			gfx::ani::AnimationSkeletonInterface* pAnimationSkeletonInterface;
			gfx::ani::AnimationState state;
		);
	};
}