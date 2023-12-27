#pragma once
#include "SystemManager.h"
#include <Core/src/utl/Span.h>
#include <Core/src/ecs/cmp/PositionComponent.h>
#include <Core/src/ecs/cmp/VelocityComponent.h>
#include <Core/src/utl/Stopwatch.h>

namespace tryn::ecs::sys
{
	ZT_DEFINE_SYSTEM(UpdatePositionSystem)
	{
		ZT_SYSTEM_UID;
		static void Execute();
	private:
		static inline utl::MultiSpan<cmp::PositionComponent::SubresourceData> positionArray;
		static inline utl::MultiSpan<cmp::VelocityComponent::SubresourceData> velocityArray;
		static inline utl::Stopwatch sw;
	};
}