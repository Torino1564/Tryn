#pragma once
#include "SystemManager.h"
#include <Core/src/utl/Span.h>
#include <Core/src/ecs/cmp/Components.h>
#include <Core/src/utl/Stopwatch.h>

namespace tryn::ecs
{
	ZT_DEFINE_SYSTEM(UpdatePositionSystem)
	{
	public:
		UpdatePositionSystem(const SystemGraph& pGraph);
		void Execute() override;

	private:
		utl::MultiSpan<PositionComponent> positionArray;
		utl::MultiSpan<VelocityComponent> velocityArray;
		utl::Stopwatch sw;
	};
}