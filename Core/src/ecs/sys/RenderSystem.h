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
		ZT_SYSTEM_UID;
		RenderSystem()
		{
			AddDependency<sys::AnimationSystem>();
			AddDependency<sys::TransformSystem>();
		}
		static void OnCreate()
		{

		}
		static void Execute();
	private:
		static inline utl::MultiSpan<cmp::ModelComponent::SubresourceData> modelArray;
		static inline utl::MultiSpan<cmp::TransformComponent::SubresourceData> transformArray;
		static inline utl::MultiSpan<cmp::ActiveComponent::SubresourceData> activeArray;

		static inline utl::MultiSpan<cmp::ActiveComponent::SubresourceData> activeChildrenArray;
		static inline utl::MultiSpan<cmp::TransformComponent::SubresourceData> transformChildrenArray;
		static inline utl::MultiSpan<cmp::InstancedModelChildComponent::SubresourceData> childrenModelArray;

		static inline utl::MultiSpan<cmp::ActiveComponent::SubresourceData> activeParentArray;
		static inline utl::MultiSpan<cmp::TransformComponent::SubresourceData> transformParentArray;
		static inline utl::MultiSpan<cmp::InstancedModelParentComponent::SubresourceData> parentModelArray;

		ZT_NATIVE_ARRAY(ActiveComponent) activeSkinnedArray;
		ZT_NATIVE_ARRAY(TransformComponent) transformSkinnedArray;
		ZT_NATIVE_ARRAY(BoneTransformsComponent) boneTransformArray;
		ZT_NATIVE_ARRAY(ModelComponent) skinnedModelArray;

		ZT_NATIVE_ARRAY(PointLightComponent)	pointLightArray;
		ZT_NATIVE_ARRAY(PositionComponent)		pointLightPositionArray;
		ZT_NATIVE_ARRAY(ActiveComponent)		pointLightActiveArray;

		static inline gfx::IGraphics* pGfx;
	};
}