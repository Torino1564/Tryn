#include "TrynPCH.h"
#include "RenderIDInfoSystem.h"

#include "AnimationSystem.h"
#include "RenderSystem.h"
#include "TransformSystem.h"
#include "Core/src/ecs/Archetype_def.h"
#include "Core/src/gfx/IGraphics.h"
#include "Core/src/gfx/Model/Model.h"
#include "Core/src/gfx/Render/Jobs/EntityIDJob.h"

namespace tryn::ecs
{
	RenderIDInfoSystem::RenderIDInfoSystem(const SystemGraph& graph) : SystemImpl(graph)
	{
	}

	void RenderIDInfoSystem::InitDependencies(System* self)
	{
		AddDependency<TransformSystem>();
		AddDependency<AnimationSystem>();

		AddPrerequisiteOf<RenderSystem>();
	}

	void RenderIDInfoSystem::Execute()
	{
		auto& archetypeManager = pEcs->GetArchetypeManager();
		auto& renderGraph = pEcs->Gfx().GetRenderGraph();

		// Request Data
		auto [archetypeIds, data] = archetypeManager.GetComponentGroupsEx<ReadOnly<ModelComponent>, ReadOnly<TransformComponent>, ReadOnly<ActiveComponent>>();

		for (auto [archetypeIdIndex, archetypeData]: data | std::views::enumerate)
		{
			const auto archetypeID = archetypeIds[archetypeIdIndex];
			auto models = std::get<std::span<ModelComponent>>(archetypeData);
			auto transforms = std::get<std::span<TransformComponent>>(archetypeData);
			auto active = std::get<std::span<ActiveComponent>>(archetypeData);

			for (auto [entityID, active] : active | std::views::enumerate)
			{
				if (!active.active)
					continue;
				models[entityID].pModel->AddOrEnableTechniques({ std::array{ZT_TYPE_UUID(EntityIDTechnique)}, 1 });
			}
		}

	}
}
