#include "Entity.h"

namespace tryn::ent
{
	void IEntity::Draw()
	{
		model->Draw();
	}
	BasicEntity::BasicEntity(gfx::IGraphics& gfx, std::string_view name, std::string_view path, glm::vec3 scale)
	{
		this->name = name;
		model = std::make_unique<gfx::Model>(gfx, path, scale);
	}
}