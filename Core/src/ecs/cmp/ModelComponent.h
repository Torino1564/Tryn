#pragma once

namespace tryn::gfx
{
	class Model;
}

namespace tryn::ecs
{
	class ModelComponent
	{
		std::unique_ptr<gfx::Model> pModel;
	};
}