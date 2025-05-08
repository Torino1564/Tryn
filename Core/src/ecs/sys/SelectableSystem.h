#pragma once
#include <Core/src/ecs/sys/SystemManager.h>

#include "Core/src/utl/Span.h"

namespace tryn::ecs
{
	ZT_DEFINE_SYSTEM(SelectableSystem)
	{
	public:
		SelectableSystem(const SystemGraph& graph);
		static void InitDependencies(System* self);

		void Execute() override;

	private:
		utl::MultiSpan<class ModelComponent> modelArray;
		utl::MultiSpan<class ActiveComponent> activeArray;
		utl::MultiSpan<class PositionComponent> positionArray;
	};
}
