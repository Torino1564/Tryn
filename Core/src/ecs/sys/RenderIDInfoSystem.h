#pragma once
#include <Core/src/ecs/sys/SystemManager.h>

#include "Core/src/utl/Span.h"

namespace tryn::ecs
{
	struct ModelComponent;
}

namespace tryn::ecs
{
	struct ActiveComponent;
	struct TransformComponent;

	class RenderIDInfoSystem : SystemImpl<RenderIDInfoSystem>
	{
	public:
		RenderIDInfoSystem(const SystemGraph& graph);
		static void InitDependencies(System* self);
		void Execute() override;
	};
}
