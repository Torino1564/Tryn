#pragma once
#include <memory>
#include <Core/src/gfx/IGraphics.h>

namespace tryn::gfx
{
	class Node;

	class Model
	{
	public:
		Model(gfx::IGraphics& gfx, std::string_view path, glm::vec3 scale = {1.0f,1.0f,1.0f});

	private:
		std::unique_ptr<Node> root;
	};
}
