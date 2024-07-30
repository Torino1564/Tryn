#include "TrynPCH.h"
#include "RenderSystem.h"
#include <Core/src/ecs/cmp/ComponentManager.h>
#include <Core/src/gfx/Render/Jobs/PointLightJob.h>
#include <Core/src/gfx/Model/InstancedModel.h>
#include <Core/src/gfx/Bindables/IBuffer.h>
#include <Core/src/gfx/Model/Model.h>

namespace tryn::ecs::sys
{
	void RenderSystem::Execute()
	{
		// Request data

		auto data = ECS::Get().archetypeManager.GetComponentGroups<
			ReadOnly<cmp::TransformComponent>,
			ReadOnly<cmp::ActiveComponent>,
			WriteOnly<cmp::ModelComponent>
		>();

		auto dataChildren = ECS::Get().archetypeManager.GetComponentGroups<
			WriteOnly<cmp::InstancedModelChildComponent>,
			ReadOnly<cmp::TransformComponent>,
			ReadOnly<cmp::ActiveComponent>>();

		auto dataParents = ECS::Get().archetypeManager.GetComponentGroups<
			WriteOnly<cmp::InstancedModelParentComponent>,
			ReadOnly<cmp::TransformComponent>,
			ReadOnly<cmp::ActiveComponent>
		>();

		auto dataSkinned = ECS::Get().archetypeManager.GetComponentGroups<
			ReadOnly<cmp::ActiveComponent>,
			ReadOnly<cmp::TransformComponent>,
			ReadOnly<cmp::BoneTransformsComponent>,
			ReadWrite<cmp::ModelComponent>
		>();

		auto dataPointLight = ECS::Get().archetypeManager.GetComponentGroups<
			ReadOnly<cmp::ActiveComponent>,
			ReadOnly<cmp::PositionComponent>,
			ReadOnly<cmp::PointLightComponent>
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
			transformArray.PushBack(std::get<std::span<cmp::TransformComponent::SubresourceData>>(queriedData));
			modelArray.PushBack(std::get<std::span<cmp::ModelComponent::SubresourceData>>(queriedData));
			activeArray.PushBack(std::get<std::span<cmp::ActiveComponent::SubresourceData>>(queriedData));
		}

		for (auto& queriedData : dataChildren)
		{
			activeChildrenArray.PushBack(std::get<std::span<cmp::ActiveComponent::SubresourceData>>(queriedData));
			transformChildrenArray.PushBack(std::get<std::span<cmp::TransformComponent::SubresourceData>>(queriedData));
			childrenModelArray.PushBack(std::get<std::span<cmp::InstancedModelChildComponent::SubresourceData>>(queriedData));
		}

		for (auto& queriedData : dataParents)
		{
			activeParentArray.PushBack(std::get<std::span<cmp::ActiveComponent::SubresourceData>>(queriedData));
			parentModelArray.PushBack(std::get<std::span<cmp::InstancedModelParentComponent::SubresourceData>>(queriedData));
			transformParentArray.PushBack(std::get<std::span<cmp::TransformComponent::SubresourceData>>(queriedData));
		}

		for (auto& queriedData : dataSkinned)
		{
			activeSkinnedArray.PushBack(std::get<std::span<cmp::ActiveComponent::SubresourceData>>(queriedData));
			boneTransformArray.PushBack(std::get<std::span<cmp::BoneTransformsComponent::SubresourceData>>(queriedData));
			transformSkinnedArray.PushBack(std::get<std::span<cmp::TransformComponent::SubresourceData>>(queriedData));
			skinnedModelArray.PushBack(std::get<std::span<cmp::ModelComponent::SubresourceData>>(queriedData));
		}

		for (auto& pointLightData : dataPointLight)
		{
			pointLightArray.PushBack(std::get<std::span<cmp::PointLightComponent::SubresourceData>>(pointLightData));
			pointLightPositionArray.PushBack(std::get<std::span<cmp::PositionComponent::SubresourceData>>(pointLightData));
			pointLightActiveArray.PushBack(std::get<std::span<cmp::ActiveComponent::SubresourceData>>(pointLightData));
		}

		if (pGfx == nullptr)
		{
			pGfx = modelArray[0].pModel->GetGfx();
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

		auto& renderGraph = pGfx->GetRenderGraph();

		for (auto i = 0; i < pointLightActiveArray.Size(); i++)
		{
			if (!pointLightActiveArray[i].active)
				continue;

			renderGraph.GetRenderQueueByID("PointLightBind").Push<gfx::PointLightJob>(pointLightArray[i].parameters, pointLightPositionArray[i].position, renderGraph);
		}
	}
}

