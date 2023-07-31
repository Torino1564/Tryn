#include "Model.h"

namespace tryn::gfx
{
	Model::Model(gfx::IGraphics & gfx, std::string_view path, glm::vec3 scale)
	{
		root = std::make_unique<Node>();
	}
}

