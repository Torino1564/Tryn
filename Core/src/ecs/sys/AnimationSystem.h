#pragma once
#include "SystemManager.h"
#include <Core/src/ecs/cmp/Components.h>
#include <Core/src/ecs/sys/TransformSystem.h>
#include <chrono>

namespace tryn::ecs
{
	ZT_DEFINE_SYSTEM(AnimationSystem)
	{
	public:
		AnimationSystem(const SystemGraph& pGraph, const ECS* pEcs);
		static void InitDependencies(System* self);
		void Execute() override;
		void Init() override;

	private:
		// state
		std::chrono::high_resolution_clock::time_point previous;
		double deltaTime = 0;
		// data
		ZT_NATIVE_ARRAY(AnimatedComponent) animatedArray;
		ZT_NATIVE_ARRAY(BoneTransformsComponent) boneTransformsArray;
		ZT_NATIVE_ARRAY(TransformComponent) transformsArray;
		ZT_NATIVE_ARRAY(ActiveComponent) activeArray;
	};
}