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
		RenderSystem(const SystemGraph& pGraph, ECS* pEcs);
		static void InitDependencies(System* self);
		void Execute() override;

	private:
		utl::MultiSpan<ModelComponent> modelArray;
		utl::MultiSpan<TransformComponent> transformArray;
		utl::MultiSpan<ActiveComponent> activeArray;

		utl::MultiSpan<ActiveComponent> activeChildrenArray;
		utl::MultiSpan<TransformComponent> transformChildrenArray;
		utl::MultiSpan<InstancedModelChildComponent> childrenModelArray;

		utl::MultiSpan<ActiveComponent> activeParentArray;
		utl::MultiSpan<TransformComponent> transformParentArray;
		utl::MultiSpan<InstancedModelParentComponent> parentModelArray;

		ZT_NATIVE_ARRAY(ActiveComponent) activeSkinnedArray;
		ZT_NATIVE_ARRAY(TransformComponent) transformSkinnedArray;
		ZT_NATIVE_ARRAY(BoneTransformsComponent) boneTransformArray;
		ZT_NATIVE_ARRAY(ModelComponent) skinnedModelArray;

		ZT_NATIVE_ARRAY(PointLightComponent)	pointLightArray;
		ZT_NATIVE_ARRAY(PositionComponent)		pointLightPositionArray;
		ZT_NATIVE_ARRAY(ActiveComponent)		pointLightActiveArray;
	};
}