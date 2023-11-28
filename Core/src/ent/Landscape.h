#pragma once
#include <Core/third/glm/glm.hpp>
#include <memory>
#include <optional>
#include <Core/src/gfx/TexturePool.h>
#include <Core/src/gfx/Model/Model.h>

namespace tryn::ent
{
	class Landscape
	{
	public:
		Landscape(std::string_view texturePath, std::optional<glm::vec3> scale = std::nullopt);
	private:
		std::shared_ptr<gfx::Texture> pTexture;
		std::unique_ptr<gfx::Model> pModel;
	};
}