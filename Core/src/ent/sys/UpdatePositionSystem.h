#pragma once
#include "SystemManager.h"
#include <Core/src/utl/Span.h>
#include <Core/src/ent/Component/PositionComponent.h>
#include <Core/src/ent/Component/VelocityComponent.h>
#include <Core/src/utl/Stopwatch.h>

namespace tryn::ent::sys
{
	ZT_DEFINE_SYSTEM(UpdatePositionSystem)
	{
		ZT_SYSTEM_UID;
		static void Execute();
	private:
		static inline utl::MultiSpan<cmp::PositionComponent::SubresourceData> positionArray;
		static inline utl::MultiSpan<cmp::VelocityComponent::SubresourceData> velocityArray;
		static utl::Stopwatch sw;
	};
}