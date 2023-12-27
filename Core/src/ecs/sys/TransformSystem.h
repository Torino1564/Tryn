#pragma once
#include <Core/src/ecs/sys/SystemManager.h>
#include <Core/src/ecs/cmp/PositionComponent.h>
#include <Core/src/ecs/cmp/ScaleComponent.h>
#include <Core/src/ecs/cmp/RotationComponent.h>
#include <Core/src/ecs/cmp/TransformComponent.h>
#include <Core/src/ecs/cmp/ActiveComponent.h>

namespace tryn::ecs::sys
{
	ZT_DEFINE_SYSTEM(TransformSystem)
	{
		ZT_SYSTEM_UID;
	public:
		TransformSystem();
		static void Execute();
	private:
		static inline utl::MultiSpan<cmp::PositionComponent::SubresourceData> positionArray;
		static inline utl::MultiSpan<cmp::ScaleComponent::SubresourceData> scaleArray;
		static inline utl::MultiSpan<cmp::RotationComponent::SubresourceData> rotationArray;
		static inline utl::MultiSpan<cmp::TransformComponent::SubresourceData> transformArray;
		static inline utl::MultiSpan<cmp::ActiveComponent::SubresourceData> activeArray;
	};
}
