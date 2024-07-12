#pragma once
#include "ComponentManager.h"

namespace tryn::gfx
{
	class Model;
}

namespace tryn::ecs::cmp
{
	ZT_DEFINE_COMPONENT(ModelComponent)
	{
		ZT_COMPONENT_FIELDS(
			SubresourceData();
			~SubresourceData();
			SubresourceData operator=(const SubresourceData&) = delete;
			ZT_DEFINE_COMPONENT_VAR(std::unique_ptr<gfx::Model>, pModel, nullptr);
		);
	};
}