#pragma once
#include <Core/src/gfx/Animation/AnimationState.h>

namespace tryn::gfx::ani
{
	struct AnimationSkeletonInterface;
}

namespace tryn::ecs
{

	struct AnimatedComponent
	{
		double time;
		uint32_t previousKey;
		gfx::ani::AnimationSkeletonInterface* pAnimationSkeletonInterface;
		gfx::ani::AnimationState state;
	};
}