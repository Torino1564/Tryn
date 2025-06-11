#pragma once
#include <Core/src/ecs/sys/SystemManager.h>

namespace tryn::ecs
{
	class HoverEntitySystem : public SystemImpl<HoverEntitySystem>
	{
	public:
		HoverEntitySystem(const SystemGraph& graph);
		static void InitDependencies(System* self);
		void Execute() override;
	};
}