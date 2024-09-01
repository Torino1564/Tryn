#pragma once
#include "SystemManager.h"
#include <Core/src/utl/Span.h>
#include <Core/src/ecs/cmp/Components.h>

#include "AnimationSystem.h"

namespace tryn::ecs
{
	ZT_DEFINE_SYSTEM(RenderSystem)
	{
	public:
		RenderSystem(const SystemGraph & pGraph);
		static void InitDependencies(System* self);
		void Execute() override;

	private:
		utl::MultiSpan<ModelComponent::SubresourceData> modelArray;
		utl::MultiSpan<TransformComponent::SubresourceData> transformArray;
		utl::MultiSpan<ActiveComponent::SubresourceData> activeArray;

		utl::MultiSpan<ActiveComponent::SubresourceData> activeChildrenArray;
		utl::MultiSpan<TransformComponent::SubresourceData> transformChildrenArray;
		utl::MultiSpan<InstancedModelChildComponent::SubresourceData> childrenModelArray;

		utl::MultiSpan<ActiveComponent::SubresourceData> activeParentArray;
		utl::MultiSpan<TransformComponent::SubresourceData> transformParentArray;
		utl::MultiSpan<InstancedModelParentComponent::SubresourceData> parentModelArray;

		ZT_NATIVE_ARRAY(ActiveComponent) activeSkinnedArray;
		ZT_NATIVE_ARRAY(TransformComponent) transformSkinnedArray;
		ZT_NATIVE_ARRAY(BoneTransformsComponent) boneTransformArray;
		ZT_NATIVE_ARRAY(ModelComponent) skinnedModelArray;

		ZT_NATIVE_ARRAY(PointLightComponent)	pointLightArray;
		ZT_NATIVE_ARRAY(PositionComponent)		pointLightPositionArray;
		ZT_NATIVE_ARRAY(ActiveComponent)		pointLightActiveArray;
	};
}