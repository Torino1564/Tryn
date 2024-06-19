#pragma once
#include "SystemManager.h"
#include <Core/src/utl/Span.h>
#include <Core/src/ecs/cmp/Components.h>

#include <Core/src/utl/Stopwatch.h>

namespace tryn::ecs::sys
{
	ZT_DEFINE_SYSTEM(UpdateVelocitySystem)
	{
		ZT_SYSTEM_UID;
		static void Execute();
	private:
		static inline utl::MultiSpan<cmp::VelocityComponent::SubresourceData> velocityArray;
		static inline utl::MultiSpan<cmp::AccelerationComponent::SubresourceData> accelerationArray;
		static inline utl::Stopwatch sw;
	};
}