#include "TrynPCH.h"
#include "RenderSystem.h"
#include <Core/src/ecs/cmp/ComponentManager.h>
#include <Core/src/gfx/Render/Jobs/PointLightJob.h>
#include <Core/src/gfx/Model/InstancedModel.h>
#include <Core/src/gfx/Bindables/IBuffer.h>
#include <Core/src/gfx/Model/Model.h>
#include <Core/src/ecs/Archetype.h>

#include "TransformSystem.h"

namespace tryn::ecs
{
	RenderSystem::RenderSystem(const SystemGraph& pGraph): SystemImpl(pGraph)
	{}

	void RenderSystem::InitDependencies(System* self)
	{
		self->AddDependency<AnimationSystem>();
		self->AddDependency<TransformSystem>();
	}

	void RenderSystem::Execute()
	{
		auto& archetypeManager = pEcs->GetArchetypeManager();

		// Request data

		auto data = archetypeManager.GetComponentGroups<
			ReadOnly<TransformComponent>,
			ReadOnly<ActiveComponent>,
			WriteOnly<ModelComponent>
		>();

		auto dataChildren = archetypeManager.GetComponentGroups<
			WriteOnly<InstancedModelChildComponent>,
			ReadOnly<TransformComponent>,
			ReadOnly<ActiveComponent>>();

		auto dataParents = archetypeManager.GetComponentGroups<
			WriteOnly<InstancedModelParentComponent>,
			ReadOnly<TransformComponent>,
			ReadOnly<ActiveComponent>
		>();

		auto dataSkinned = archetypeManager.GetComponentGroups<
			ReadOnly<ActiveComponent>,
			ReadOnly<TransformComponent>,
			ReadOnly<BoneTransformsComponent>,
			ReadWrite<ModelComponent>
		>();

		auto dataPointLight = archetypeManager.GetComponentGroups<
			ReadOnly<ActiveComponent>,
			ReadOnly<PositionComponent>,
			ReadOnly<PointLightComponent>
		>();

		// Reset data arrays

		transformArray.Clear();
		modelArray.Clear();
		activeArray.Clear();

		transformChildrenArray.Clear();
		activeChildrenArray.Clear();
		childrenModelArray.Clear();

		transformParentArray.Clear();
		activeParentArray.Clear();
		parentModelArray.Clear();

		activeSkinnedArray.Clear();
		transformSkinnedArray.Clear();
		boneTransformArray.Clear();

		pointLightArray.Clear();
		pointLightPositionArray.Clear();
		pointLightActiveArray.Clear();

		// Fill data arrays

		for (auto& queriedData : data)
		{
			transformArray.PushBack(std::get<std::span<TransformComponent>>(queriedData));
			modelArray.PushBack(std::get<std::span<ModelComponent>>(queriedData));
			activeArray.PushBack(std::get<std::span<ActiveComponent>>(queriedData));
		}

		for (auto& queriedData : dataChildren)
		{
			activeChildrenArray.PushBack(std::get<std::span<ActiveComponent>>(queriedData));
			transformChildrenArray.PushBack(std::get<std::span<TransformComponent>>(queriedData));
			childrenModelArray.PushBack(std::get<std::span<InstancedModelChildComponent>>(queriedData));
		}

		for (auto& queriedData : dataParents)
		{
			activeParentArray.PushBack(std::get<std::span<ActiveComponent>>(queriedData));
			parentModelArray.PushBack(std::get<std::span<InstancedModelParentComponent>>(queriedData));
			transformParentArray.PushBack(std::get<std::span<TransformComponent>>(queriedData));
		}

		for (auto& queriedData : dataSkinned)
		{
			activeSkinnedArray.PushBack(std::get<std::span<ActiveComponent>>(queriedData));
			boneTransformArray.PushBack(std::get<std::span<BoneTransformsComponent>>(queriedData));
			transformSkinnedArray.PushBack(std::get<std::span<TransformComponent>>(queriedData));
			skinnedModelArray.PushBack(std::get<std::span<ModelComponent>>(queriedData));
		}

		for (auto& pointLightData : dataPointLight)
		{
			pointLightArray.PushBack(std::get<std::span<PointLightComponent>>(pointLightData));
			pointLightPositionArray.PushBack(std::get<std::span<PositionComponent>>(pointLightData));
			pointLightActiveArray.PushBack(std::get<std::span<ActiveComponent>>(pointLightData));
		}

		// Kernel

		for (auto i = 0; i < modelArray.Size(); i++)
		{
			if (activeArray[i].active)
			{
				modelArray[i].pModel->Submit(transformArray[i].transform);
			}
		}

		for (auto i = 0; i < childrenModelArray.Size(); i++)
		{
			if (activeChildrenArray[i].active)
			{
				childrenModelArray[i].childModel.Submit(transformChildrenArray[i].transform);
			}
		}

		for (auto i = 0; i < parentModelArray.Size(); i++)
		{
			if (activeParentArray[i].active)
			{
				parentModelArray[i].pParentModel->Submit(transformParentArray[i].transform);
			}
		}

		for (auto i = 0; i < activeSkinnedArray.Size(); i++)
		{
			if (!activeSkinnedArray[i].active)
				continue;

			skinnedModelArray[i].pModel->Submit(transformSkinnedArray[i].transform, boneTransformArray[i].transforms);
		}

		// Get handle to render graph

		auto& renderGraph = pGraph->Gfx().GetRenderGraph();

		for (auto i = 0; i < pointLightActiveArray.Size(); i++)
		{
			if (!pointLightActiveArray[i].active)
				continue;

			renderGraph.GetRenderQueueByID("PointLightBind").Push<gfx::PointLightJob>(pointLightArray[i].parameters, pointLightPositionArray[i].position, renderGraph);
		}
	}
}

