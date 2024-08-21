#pragma once
#include "SystemManager.h"
#include <Core/src/utl/Span.h>
#include <Core/src/ecs/cmp/Components.h>
#include <Core/src/utl/Stopwatch.h>

namespace tryn::ecs::sys
{
	ZT_DEFINE_SYSTEM(UpdatePositionSystem)
	{
	public:
		UpdatePositionSystem(const SystemGraph & pGraph);
		void Execute() override;

	private:
		utl::MultiSpan<cmp::PositionComponent::SubresourceData> positionArray;
		utl::MultiSpan<cmp::VelocityComponent::SubresourceData> velocityArray;
		utl::Stopwatch sw;
	};
}