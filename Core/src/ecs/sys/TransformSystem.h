#pragma once
#include <Core/src/ecs/sys/SystemManager.h>
#include <Core/src/ecs/cmp/Components.h>
#include "Core/src/utl/Span.h"


namespace tryn::ecs
{
	ZT_DEFINE_SYSTEM(TransformSystem)
	{
	public:
		TransformSystem(const SystemGraph& pGraph);
		static void InitDependencies(System* self);
		void Execute() override;

	private:
		utl::MultiSpan<PositionComponent> positionArray;
		utl::MultiSpan<ScaleComponent> scaleArray;
		utl::MultiSpan<RotationComponent> rotationArray;
		utl::MultiSpan<TransformComponent> transformArray;
		utl::MultiSpan<ActiveComponent> activeArray;
	};
}
