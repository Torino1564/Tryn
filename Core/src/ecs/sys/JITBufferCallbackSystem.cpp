
#include "JITBufferCallbackSystem.h"

#include "Core/src/ecs/Archetype_def.h"
#include "Core/src/gfx/IGraphics.h"

#include <Core/src/ecs/cmp/UpdateJITBufferComponent.h>
#include <Core/src/ecs/cmp/ActiveComponent.h>
#include "RenderSystem.h"
#include <Core/src/ecs/Archetype_impl.h>

namespace tryn::ecs
{
	JITBufferCallbackSystem::JITBufferCallbackSystem(const SystemGraph& graph) : SystemImpl(graph)
	{
	}

	void JITBufferCallbackSystem::InitDependencies(System* self)
	{
		self->AddPrerequisiteOf<RenderSystem>();
	}

	void JITBufferCallbackSystem::Execute()
	{
		auto& archetypeManager = pEcs->GetArchetypeManager();
		auto& renderGraph = pEcs->Gfx().GetRenderGraph();

		// Request Data
		auto [archetypeIds, data] = archetypeManager.GetComponentGroupsEx<ReadOnly<ActiveComponent>, ReadOnly<UpdateJITBufferComponent>>();

		for (auto [archetypeIdIndex, archetypeData]: data | std::views::enumerate)
		{
			const auto archetypeID =  archetypeIds[archetypeIdIndex];
			auto& archetype = archetypeManager.GetArchetype(archetypeID);
			const auto& active = std::get<std::span<ActiveComponent>>(archetypeData);
			const auto& jitComponent = std::get<std::span<UpdateJITBufferComponent>>(archetypeData);

			for (const auto [entityID, active] : active | std::views::enumerate)
			{
				if (active.active)
				{
					for (auto& [pBuffer, pFunc] : jitComponent[entityID].jitCombinations)
					{
						pFunc(pBuffer, entityID, &archetype);
					}
				}
			}
		}
	}
}
