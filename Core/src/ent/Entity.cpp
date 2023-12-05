#include "Entity.h"
#include <Core/src/ent/Components/Component.h>
#include <format>

namespace tryn::ent
{
	void IEntity::SpawnControlWindow()
	{
		if (ImGui::Begin(std::format("Entity [{}]", name).c_str()))
		{
			componentManager.ComponentTree();
		}
	}
	void IEntity::Update(double dt)
	{
		componentManager.Update(dt);
	}
	BasicEntity::BasicEntity(gfx::IGraphics& gfx, std::string_view name, std::string_view path, glm::vec3 scale)
	{
		this->name = name;
	}
}