#include "Entity.h"

namespace tryn::ent
{
	void IEntity::Submit()
	{
		const auto rotation = glm::yawPitchRoll(settings.angles.x, settings.angles.y, settings.angles.z);
		const auto translation = glm::translate(glm::mat4(1.0f), settings.position);
		transform = translation * rotation;
		model->Submit(transform);
	}
	void IEntity::SpawnControlWindow()
	{
		ImGui::Begin(name.c_str());
		ImGui::Text("Entity Settings");
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Yaw", &settings.angles.x, -180.f, 180.f);
		ImGui::SliderAngle("Pitch", &settings.angles.y, -90.f, 90.f);
		ImGui::SliderAngle("Roll", &settings.angles.z, -180.f, 180.f);
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &settings.position.x, -20.0f, 20.0f);
		ImGui::SliderFloat("Y", &settings.position.y, -20.0f, 20.0f);
		ImGui::SliderFloat("Z", &settings.position.z, -20.0f, 20.0f);
		ImGui::End();
	}
	BasicEntity::BasicEntity(gfx::IGraphics& gfx, std::string_view name, std::string_view path, glm::vec3 scale)
	{
		this->name = name;
		model = std::make_unique<gfx::Model>(gfx, path, scale);
	}
}