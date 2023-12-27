#pragma once
#include <Core/src/ent/sys/SystemManager.h>
#include <Core/src/ent/Component/PositionComponent.h>
#include <Core/src/ent/Component/ScaleComponent.h>
#include <Core/src/ent/Component/RotationComponent.h>
#include <Core/src/ent/Component/TransformComponent.h>
#include <Core/src/ent/Component/ActiveComponent.h>

namespace tryn::ent::sys
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
