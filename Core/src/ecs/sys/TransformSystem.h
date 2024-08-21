#pragma once
#include <Core/src/ecs/sys/SystemManager.h>

#include <Core/src/ecs/cmp/Components.h>


namespace tryn::ecs::sys
{
	ZT_DEFINE_SYSTEM(TransformSystem)
	{
	public:
		TransformSystem(const SystemGraph& pGraph);
		static void InitDependencies(System* self);
		void Execute() override;

	private:
		utl::MultiSpan<cmp::PositionComponent::SubresourceData> positionArray;
		utl::MultiSpan<cmp::ScaleComponent::SubresourceData> scaleArray;
		utl::MultiSpan<cmp::RotationComponent::SubresourceData> rotationArray;
		utl::MultiSpan<cmp::TransformComponent::SubresourceData> transformArray;
		utl::MultiSpan<cmp::ActiveComponent::SubresourceData> activeArray;
	};
}
