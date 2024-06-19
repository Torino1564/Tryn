#pragma once
#include "SystemManager.h"
#include <Core/src/ecs/cmp/Components.h>
#include <Core/src/ecs/sys/TransformSystem.h>
#include <chrono>

namespace tryn::ecs::sys
{
	ZT_DEFINE_SYSTEM(AnimationSystem)
	{
	public:
		ZT_SYSTEM_UID;
		AnimationSystem()
		{
			AddDependency<TransformSystem>();
		}
		static void Execute();
		static void OnCreate();
	private:
		// state
		static inline std::chrono::high_resolution_clock::time_point previous;
		static inline double deltaTime = 0;
		// data
		ZT_NATIVE_ARRAY(AnimatedComponent) animatedArray;
		ZT_NATIVE_ARRAY(BoneTransformsComponent) boneTransformsArray;
		ZT_NATIVE_ARRAY(TransformComponent) transformsArray;
		ZT_NATIVE_ARRAY(ActiveComponent) activeArray;
	};
}