#pragma once
#include "SystemManager.h"
#include <Core/src/ecs/cmp/ModelComponent.h>
#include <Core/src/ecs/cmp/TransformComponent.h>
#include <Core/src/ecs/cmp/ActiveComponent.h>
#include <Core/src/utl/Span.h>
#include <Core/src/ecs/sys/TransformSystem.h>

namespace tryn::ecs::sys
{
	ZT_DEFINE_SYSTEM(RenderSystem)
	{
	public:
		ZT_SYSTEM_UID;
		RenderSystem()
		{
			AddDependency<sys::TransformSystem>();
		}
		static void Execute();
	private:
		static inline utl::MultiSpan<cmp::ModelComponent::SubresourceData> modelArray;
		static inline utl::MultiSpan<cmp::TransformComponent::SubresourceData> transformArray;
		static inline utl::MultiSpan<cmp::ActiveComponent::SubresourceData> activeArray;
	};
}