#pragma once
#include <memory>
#include "Node.h"
#include <Core/src/gfx/IGraphics.h>
#include "Mesh.h"
#include <Core/src/gfx/ImguiManager.h>

namespace tryn::gfx
{
	class Model
	{
	public:
		Model(gfx::IGraphics& gfx, std::string_view path, glm::vec3 scale = { 1.0f,1.0f,1.0f });
		void Draw();
		void SpawnControlWindow();
	private:
		Node ParseNode(int& nextId, const aiNode& node, glm::vec3 scale);
		struct Settings
		{
			glm::vec3 angles; // (yaw, pitch, roll)
			glm::vec3 position; // (x, y, z)
		};
	private:
		Settings settings = {};
		gfx::IGraphics& gfx;
		std::string name;
		std::unique_ptr<Node> root;
		std::vector<std::shared_ptr<Mesh>> pMeshes;
	};
}
