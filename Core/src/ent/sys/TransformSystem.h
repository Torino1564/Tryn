#pragma once
#include <Core/src/ent/sys/SystemManager.h>
#include <Core/src/ent/Component/PositionComponent.h>
#include <Core/src/ent/Component/ScaleComponent.h>
#include <Core/src/ent/Component/RotationComponent.h>
#include <Core/src/ent/Component/TransformComponent.h>

namespace tryn::ent::sys
{
	ZT_DEFINE_SYSTEM(TransformSystem)
	{
		ZT_SYSTEM_UID;
	public:
		TransformSystem() = default;
		static void Execute();
	private:
		static utl::MultiSpan<cmp::PositionComponent::SubresourceData> positionArray;
		static utl::MultiSpan<cmp::ScaleComponent::SubresourceData> scaleArray;
		static utl::MultiSpan<cmp::RotationComponent::SubresourceData> rotationArray;
		static utl::MultiSpan<cmp::TransformComponent::SubresourceData> transformArray;
	};
}
