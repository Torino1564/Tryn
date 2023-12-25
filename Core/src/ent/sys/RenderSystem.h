#pragma once
#include "SystemManager.h"
#include <Core/src/ent/Component/ModelComponent.h>
#include <Core/src/ent/Component/TransformComponent.h>
#include <Core/src/utl/Span.h>

namespace tryn::ent::sys
{
	ZT_DEFINE_SYSTEM(RenderSystem)
	{
	public:
		ZT_SYSTEM_UID;
		RenderSystem()
		{
			//AddDependency<cmp::PhysicsSystem>();
		}
		static void Execute();
	private:
		static utl::MultiSpan<cmp::ModelComponent::SubresourceData> model;
		static utl::MultiSpan<cmp::TransformComponent::SubresourceData> transform;
	};
}