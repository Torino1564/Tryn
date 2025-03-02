#pragma once
#include "SystemManager.h"
#include <Core/src/ecs/cmp/Components.h>
#include <Core/src/ecs/sys/TransformSystem.h>
#include <chrono>
#include <Core/src/utl/Span.h>

namespace tryn::ecs
{
	ZT_DEFINE_SYSTEM(AnimationSystem)
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
		private: tryn::utl::MultiSpan<AnimatedComponent> animatedArray;
		private: tryn::utl::MultiSpan<BoneTransformsComponent> boneTransformsArray;
		private: tryn::utl::MultiSpan<TransformComponent> transformsArray;
		private: tryn::utl::MultiSpan<ActiveComponent> activeArray;
	};
}