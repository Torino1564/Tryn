#pragma once
#include "ComponentManager.h"
#include <Core/src/gfx/Animation/AnimationState.h>

namespace tryn::gfx::ani
{
	struct AnimationSkeletonInterface;
}

namespace tryn::ecs
{
	ZT_DEFINE_COMPONENT(AnimatedComponent)
	{
		ZT_COMPONENT_FIELDS(
			ZT_DEFINE_COMPONENT_VAR(double, time);
			ZT_DEFINE_COMPONENT_VAR(uint32_t, previousKey);
			ZT_DEFINE_COMPONENT_VAR(gfx::ani::AnimationSkeletonInterface*, pAnimationSkeletonInterface);
			ZT_DEFINE_COMPONENT_VAR(gfx::ani::AnimationState, state);
		);
	};
}