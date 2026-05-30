#pragma once
#include "SystemManager.h"
#include <Core/src/ecs/cmp/Components.h>
#include <chrono>
#include <Core/src/utl/Span.h>

namespace tryn::ecs
{
	class AnimationSystem : public SystemImpl<AnimationSystem>
	{
	public:
		AnimationSystem(const SystemGraph& pGraph);
		static void InitDependencies(System* self);
		void Execute() override;
		void Init() override;

	private:

		// state
		std::chrono::high_resolution_clock::time_point previous;
		double deltaTime = 0;
		
		// data
		utl::MultiSpan<AnimatedComponent> animatedArray;
		utl::MultiSpan<BoneTransformsComponent> boneTransformsArray;
		utl::MultiSpan<TransformComponent> transformsArray;
		utl::MultiSpan<ActiveComponent> activeArray;
	};
}