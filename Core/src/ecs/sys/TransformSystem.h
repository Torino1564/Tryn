#pragma once
#include <Core/src/ecs/sys/SystemManager.h>

#include <Core/src/ecs/cmp/Components.h>


namespace tryn::ecs
{
	ZT_DEFINE_SYSTEM(TransformSystem)
	{
	public:
		TransformSystem(const SystemGraph& pGraph, ECS* pEcs);
		static void InitDependencies(System* self);
		void Execute() override;

	private:
		utl::MultiSpan<PositionComponent::SubresourceData> positionArray;
		utl::MultiSpan<ScaleComponent::SubresourceData> scaleArray;
		utl::MultiSpan<RotationComponent::SubresourceData> rotationArray;
		utl::MultiSpan<TransformComponent::SubresourceData> transformArray;
		utl::MultiSpan<ActiveComponent::SubresourceData> activeArray;
	};
}
