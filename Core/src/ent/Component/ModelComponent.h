#pragma once
#include "ComponentManager.h"
#include <Core/src/gfx/Model/Model.h>
#include <string>

namespace tryn::ent::cmp
{
	ZT_DEFINE_COMPONENT(ModelComponent)
	{
		ZT_COMPONENT_FIELDS(
			std::unique_ptr<gfx::Model> pModel;
		);

		ZT_COMPONENT_CONSTRUCTOR(gfx::IGraphics & gfx, std::string path)
		{
			SubresourceData srd;
			srd.pModel = std::make_unique<gfx::Model>(gfx, path);
			return std::move(srd);
		}
	};
}