#pragma once
#include "SystemManager.h"
#include <Core/src/utl/Span.h>
#include <Core/src/ecs/cmp/Components.h>

#include "AnimationSystem.h"

namespace tryn::ecs::sys
{
	ZT_DEFINE_SYSTEM(RenderSystem)
	{
	public:
		RenderSystem(const SystemGraph & pGraph);
		static void InitDependencies(System* self);
		void Execute() override;

	private:
		utl::MultiSpan<cmp::ModelComponent::SubresourceData> modelArray;
		utl::MultiSpan<cmp::TransformComponent::SubresourceData> transformArray;
		utl::MultiSpan<cmp::ActiveComponent::SubresourceData> activeArray;

		utl::MultiSpan<cmp::ActiveComponent::SubresourceData> activeChildrenArray;
		utl::MultiSpan<cmp::TransformComponent::SubresourceData> transformChildrenArray;
		utl::MultiSpan<cmp::InstancedModelChildComponent::SubresourceData> childrenModelArray;

		utl::MultiSpan<cmp::ActiveComponent::SubresourceData> activeParentArray;
		utl::MultiSpan<cmp::TransformComponent::SubresourceData> transformParentArray;
		utl::MultiSpan<cmp::InstancedModelParentComponent::SubresourceData> parentModelArray;

		ZT_NATIVE_ARRAY(ActiveComponent) activeSkinnedArray;
		ZT_NATIVE_ARRAY(TransformComponent) transformSkinnedArray;
		ZT_NATIVE_ARRAY(BoneTransformsComponent) boneTransformArray;
		ZT_NATIVE_ARRAY(ModelComponent) skinnedModelArray;

		ZT_NATIVE_ARRAY(PointLightComponent)	pointLightArray;
		ZT_NATIVE_ARRAY(PositionComponent)		pointLightPositionArray;
		ZT_NATIVE_ARRAY(ActiveComponent)		pointLightActiveArray;
	};
}