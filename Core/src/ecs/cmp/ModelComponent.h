#pragma once

namespace tryn::gfx
{
	class Model;
}

namespace tryn::ecs
{
	struct ModelComponent
	{
		~ModelComponent();
		std::unique_ptr<gfx::Model> pModel;
	};
}